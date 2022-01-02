#include <babylon/materials/node/blocks/dual/light_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

LightBlock::LightBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , light{nullptr}
    , worldPosition{this, &LightBlock::get_worldPosition}
    , worldNormal{this, &LightBlock::get_worldNormal}
    , cameraPosition{this, &LightBlock::get_cameraPosition}
    , glossiness{this, &LightBlock::get_glossiness}
    , glossPower{this, &LightBlock::get_glossPower}
    , diffuseColor{this, &LightBlock::get_diffuseColor}
    , specularColor{this, &LightBlock::get_specularColor}
    , view{this, &LightBlock::get_view}
    , diffuseOutput{this, &LightBlock::get_diffuseOutput}
    , specularOutput{this, &LightBlock::get_specularOutput}
    , shadow{this, &LightBlock::get_shadow}
{
  _isUnique = true;

  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("cameraPosition", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("glossiness", NodeMaterialBlockConnectionPointTypes::Float, true,
                NodeMaterialBlockTargets::Fragment);
  registerInput("glossPower", NodeMaterialBlockConnectionPointTypes::Float, true,
                NodeMaterialBlockTargets::Fragment);
  registerInput("diffuseColor", NodeMaterialBlockConnectionPointTypes::Color3, true,
                NodeMaterialBlockTargets::Fragment);
  registerInput("specularColor", NodeMaterialBlockConnectionPointTypes::Color3, true,
                NodeMaterialBlockTargets::Fragment);
  registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, true);

  registerOutput("diffuseOutput", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("specularOutput", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("shadow", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
}

LightBlock::~LightBlock() = default;

std::string LightBlock::getClassName() const
{
  return "LightBlock";
}

NodeMaterialConnectionPointPtr& LightBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& LightBlock::get_worldNormal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& LightBlock::get_cameraPosition()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& LightBlock::get_glossiness()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& LightBlock::get_glossPower()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& LightBlock::get_diffuseColor()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& LightBlock::get_specularColor()
{
  return _inputs[6];
}

NodeMaterialConnectionPointPtr& LightBlock::get_view()
{
  return _inputs[7];
}

NodeMaterialConnectionPointPtr& LightBlock::get_diffuseOutput()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& LightBlock::get_specularOutput()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& LightBlock::get_shadow()
{
  return _outputs[2];
}

void LightBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!cameraPosition()->isConnected()) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->systemValue() == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition());
  }
}

void LightBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                NodeMaterialDefines& defines, bool /*useInstances*/,
                                SubMesh* /*subMesh*/)
{
  if (!defines._areLightsDirty) {
    return;
  }

  auto scene = mesh->getScene();

  if (!light) {
    MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, true,
                                            nodeMaterial->maxSimultaneousLights);
  }
  else {
    PrepareDefinesForLightsState state{
      false, // needNormals
      false, // needRebuild
      false, // shadowEnabled
      false, // specularEnabled
      false  // lightmapMode
    };

    MaterialHelper::PrepareDefinesForLight(scene, mesh, light, _lightId, defines, true, state);

    if (state.needRebuild) {
      defines.rebuild();
    }
  }
}

void LightBlock::updateUniformsAndSamples(NodeMaterialBuildState& state,
                                          const NodeMaterialPtr& nodeMaterial,
                                          const NodeMaterialDefines& defines,
                                          std::vector<std::string>& uniformBuffers)
{
  for (auto lightIndex = 0u; lightIndex < nodeMaterial->maxSimultaneousLights; ++lightIndex) {
    const auto lightIndexStr = std::to_string(lightIndex);
    if (!defines["LIGHT" + lightIndexStr]) {
      break;
    }
    const auto onlyUpdateBuffersList
      = stl_util::index_of(state.uniforms, "vLightData" + lightIndexStr) >= 0;
    MaterialHelper::PrepareUniformsAndSamplersForLight(
      lightIndex, state.uniforms, state.samplers, uniformBuffers, true,
      defines["PROJECTEDLIGHTTEXTURE" + lightIndexStr], onlyUpdateBuffersList);
  }
}

void LightBlock::bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh,
                      SubMesh* /*subMesh*/)
{
  if (!mesh) {
    return;
  }

  auto scene = mesh->getScene();

  if (!light) {
    MaterialHelper::BindLights(scene, mesh, effect, true, nodeMaterial->maxSimultaneousLights);
  }
  else {
    MaterialHelper::BindLight(light, _lightId, scene, effect, true);
  }
}

void LightBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  const auto& worldPos = worldPosition();
  auto iComments       = StringTools::printf("//%s", name().c_str());

  // Declaration
  if (!light) { // Emit for all lights
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightVxUboDeclaration" :
                                                                 "lightVxFragmentDeclaration",
                                   iComments,
                                   EmitFunctionFromIncludeOptions{
                                     "maxSimultaneousLights" // repeatKey
                                   });
    _lightId = 0;

    state.sharedData->dynamicUniformBlocks.emplace_back(this);
  }
  else {

    _lightId = static_cast<uint32_t>((stl_util::contains(state.counters, "lightCounter") ?
                                        static_cast<int>(state.counters["lightCounter"]) :
                                        -1)
                                     + 1);
    state.counters["lightCounter"] = _lightId;

    EmitFunctionFromIncludeOptions options;
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightVxUboDeclaration" :
                                                                 "lightVxFragmentDeclaration",
                                   iComments, options, std::to_string(_lightId));
  }

  // Inject code in vertex
  auto worldPosVaryingName
    = StringTools::printf("v_%s", worldPos->associatedVariableName().c_str());
  if (state._emitVaryingFromString(worldPosVaryingName, "vec4")) {
    state.compilationString += StringTools::printf("%s = %s;\r\n", worldPosVaryingName.c_str(),
                                                   worldPos->associatedVariableName().c_str());
  }

  if (light) {
    EmitCodeFromIncludeOptions options;
    options.replaceStrings = {StringsReplacement{
                                "{X}",                   // search
                                std::to_string(_lightId) // replace
                              },
                              StringsReplacement{
                                "worldPos",                        // search
                                worldPos->associatedVariableName() // replace
                              }};
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", iComments, options);
  }
  else {
    state.compilationString
      += StringTools::printf("vec4 worldPos = %s;\r\n", worldPos->associatedVariableName().c_str());
    if (view()->isConnected()) {
      state.compilationString
        += StringTools::printf("mat4 view = %s;\r\n", view()->associatedVariableName().c_str());
    }
    EmitCodeFromIncludeOptions options;
    options.repeatKey = "maxSimultaneousLights";
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", iComments, options);
  }
}

LightBlock& LightBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    _injectVertexCode(state);

    return *this;
  }

  // Fragment
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

  auto iComments       = StringTools::printf("//%s", name().c_str());
  const auto& worldPos = worldPosition();

  state._emitFunctionFromInclude("helperFunctions", iComments);

  EmitFunctionFromIncludeOptions options;
  options.replaceStrings = {StringsReplacement{
    "vPositionW",                                                               // search
    StringTools::printf("v_%s.xyz", worldPos->associatedVariableName().c_str()) // replace
  }};
  state._emitFunctionFromInclude("lightsFragmentFunctions", iComments, options);

  state._emitFunctionFromInclude("shadowsFragmentFunctions", iComments, options);

  if (!light) { // Emit for all lights
    state._emitFunctionFromInclude(
      state.supportUniformBuffers ? "lightUboDeclaration" : "lightFragmentDeclaration", iComments,
      EmitFunctionFromIncludeOptions{
        "maxSimultaneousLights" // repeatKey
      });
  }
  else {
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};

    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   iComments, options, std::to_string(_lightId));
  }

  // Code
  if (_lightId == 0) {
    if (state._registerTempVariable("viewDirectionW")) {
      state.compilationString += StringTools::printf(
        "vec3 viewDirectionW = normalize(%s - %s);\r\n",
        cameraPosition()->associatedVariableName().c_str(),
        StringTools::printf("v_%s.xyz", worldPos->associatedVariableName().c_str()).c_str());
    }
    state.compilationString += "lightingInfo info;\r\n";
    state.compilationString += "float shadow = 1.;\r\n";
    state.compilationString += StringTools::printf(
      "float glossiness = %s * %s;\r\n",
      glossiness()->isConnected() ? glossiness()->associatedVariableName().c_str() : "1.0",
      glossPower()->isConnected ? glossPower()->associatedVariableName().c_str() : "1024.0");
    state.compilationString += "vec3 diffuseBase = vec3(0., 0., 0.);\r\n";
    state.compilationString += "vec3 specularBase = vec3(0., 0., 0.);\r\n";
    state.compilationString += StringTools::printf("vec3 normalW = %s.xyz;\r\n",
                                                   worldNormal()->associatedVariableName().c_str());
  }

  if (light) {
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};
  }
  else {
    EmitCodeFromIncludeOptions includeOptions;
    includeOptions.repeatKey = "maxSimultaneousLights";
    state.compilationString
      += state._emitCodeFromInclude("lightFragment", iComments, includeOptions);
  }

  const auto& _diffuseOutput  = diffuseOutput();
  const auto& _specularOutput = specularOutput();

  state.compilationString
    += _declareOutput(_diffuseOutput, state)
       + StringTools::printf(
         " = diffuseBase%s;\r\n",
         diffuseColor()->isConnected() ?
           StringTools::printf(" * %s", diffuseColor()->associatedVariableName().c_str()).c_str() :
           "");
  if (_specularOutput->hasEndpoints()) {
    state.compilationString
      += _declareOutput(_specularOutput, state)
         + StringTools::printf(
           " = specularBase%s;\r\n",
           specularColor()->isConnected() ?
             StringTools::printf(" * %s", specularColor()->associatedVariableName().c_str())
               .c_str() :
             "");
  }

  if (shadow()->hasEndpoints()) {
    state.compilationString += _declareOutput(shadow, state) + " = shadow;\r\n";
  }

  return *this;
}

json LightBlock::serialize() const
{
  return nullptr;
}

void LightBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
