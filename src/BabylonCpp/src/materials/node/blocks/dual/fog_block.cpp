#include <babylon/materials/node/blocks/dual/fog_block.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/enums/node_material_system_values.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

FogBlock::FogBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment, false}
    , worldPosition{this, &FogBlock::get_worldPosition}
    , view{this, &FogBlock::get_view}
    , input{this, &FogBlock::get_input}
    , fogColor{this, &FogBlock::get_fogColor}
    , output{this, &FogBlock::get_output}
{
  // Vertex
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                NodeMaterialBlockTargets::Vertex);

  // Fragment
  registerInput("input", NodeMaterialBlockConnectionPointTypes::Color3, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("fogColor", NodeMaterialBlockConnectionPointTypes::Color3, false,
                NodeMaterialBlockTargets::Fragment);

  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);

  input()->acceptedConnectionPointTypes.emplace_back(NodeMaterialBlockConnectionPointTypes::Color4);
  fogColor()->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
}

FogBlock::~FogBlock() = default;

std::string FogBlock::getClassName() const
{
  return "FogBlock";
}

NodeMaterialConnectionPointPtr& FogBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& FogBlock::get_view()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& FogBlock::get_input()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& FogBlock::get_fogColor()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& FogBlock::get_output()
{
  return _outputs[0];
}

void FogBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!view()->isConnected()) {
    auto viewInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->systemValue() == NodeMaterialSystemValues::View;
    });

    if (!viewInput) {
      viewInput = InputBlock::New("view");
      viewInput->setAsSystemValue(NodeMaterialSystemValues::View);
    }
    viewInput->output()->connectTo(view);
  }
  if (!fogColor()->isConnected()) {
    auto fogColorInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->systemValue() == NodeMaterialSystemValues::FogColor;
    });

    if (!fogColorInput) {
      fogColorInput = InputBlock::New("fogColor", NodeMaterialBlockTargets::Vertex,
                                      NodeMaterialBlockConnectionPointTypes::Color3);
      fogColorInput->setAsSystemValue(NodeMaterialSystemValues::FogColor);
    }
    fogColorInput->output()->connectTo(fogColor);
  }
}

void FogBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                              NodeMaterialDefines& defines, bool /*useInstances*/,
                              SubMesh* /*subMesh*/)
{
  auto scene = mesh->getScene();
  defines.setValue("FOG", nodeMaterial->fogEnabled() && MaterialHelper::GetFogState(mesh, scene));
}

void FogBlock::bind(Effect* effect, const NodeMaterialPtr& /*nodeMaterial*/, Mesh* mesh,
                    SubMesh* /*subMesh*/)
{
  if (!mesh) {
    return;
  }

  const auto& scene = mesh->getScene();
  effect->setFloat4(_fogParameters, static_cast<float>(scene->fogMode()), scene->fogStart,
                    scene->fogEnd, scene->fogDensity);
}

FogBlock& FogBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
    state.sharedData->bindableBlocks.emplace_back(shared_from_this());

    state._emitFunctionFromInclude(
      "fogFragmentDeclaration", StringTools::printf("//%s", name().c_str()),
      EmitFunctionFromIncludeOptions{
        "",           // repeatKey
        std::nullopt, // removeAttributes
        true,         // removeUniforms
        true,         // removeVaryings
        false,        // removeIfDef
        {StringsReplacement{
          "float CalcFogFactor()",                                 // search
          "float CalcFogFactor(vec3 vFogDistance, vec4 vFogInfos)" // replace
        }}                                                         // replaceStrings
      });

    auto tempFogVariablename = state._getFreeVariableName("fog");
    const auto& color        = input();
    const auto& _fogColor    = fogColor();
    _fogParameters           = state._getFreeVariableName("fogParameters");
    const auto& iOutput      = _outputs[0];

    state._emitUniformFromString(_fogParameters, "vec4");

    state.compilationString += "#ifdef FOG\r\n";
    state.compilationString
      += StringTools::printf("float %s = CalcFogFactor(%s, %s);\r\n", tempFogVariablename.c_str(),
                             _fogDistanceName.c_str(), _fogParameters.c_str());
    state.compilationString
      += _declareOutput(iOutput, state)
         + StringTools::printf(" = %s * %s.rgb + (1.0 - %s) * %s..rgb;\r\n",
                               tempFogVariablename.c_str(), color->associatedVariableName().c_str(),
                               tempFogVariablename.c_str(),
                               _fogColor->associatedVariableName().c_str());
    state.compilationString
      += StringTools::printf("#else\r\n%s = %s.rgb;\r\n", _declareOutput(iOutput, state).c_str(),
                             color->associatedVariableName().c_str());
    state.compilationString += "#endif\r\n";
  }
  else {
    const auto& worldPos = worldPosition();
    const auto& _view    = view();
    _fogDistanceName     = state._getFreeVariableName("vFogDistance");
    state._emitVaryingFromString(_fogDistanceName, "vec3");
    state.compilationString
      += StringTools::printf("%s = (%s * %s).xyz;\r\n", _view->associatedVariableName().c_str(),
                             worldPos->associatedVariableName().c_str());
  }

  return *this;
}

} // end of namespace BABYLON
