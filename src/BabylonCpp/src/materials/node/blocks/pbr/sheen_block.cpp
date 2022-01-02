#include <babylon/materials/node/blocks/pbr/sheen_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/pbr/reflection_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

SheenBlock::SheenBlock(const std::string& iName)
    : NodeMaterialBlock(iName, NodeMaterialBlockTargets::Fragment)
    , albedoScaling{false}
    , linkSheenWithAlbedo{false}
    , intensity{this, &SheenBlock::get_intensity}
    , color{this, &SheenBlock::get_color}
    , roughness{this, &SheenBlock::get_roughness}
    , sheen{this, &SheenBlock::get_sheen}
{
  _isUnique = true;
}

SheenBlock::~SheenBlock() = default;

void SheenBlock::RegisterConnections(const SheenBlockPtr& sheenBlock)
{
  sheenBlock->registerInput("intensity", NodeMaterialBlockConnectionPointTypes::Float, true,
                            NodeMaterialBlockTargets::Fragment);
  sheenBlock->registerInput("color", NodeMaterialBlockConnectionPointTypes::Color3, true,
                            NodeMaterialBlockTargets::Fragment);
  sheenBlock->registerInput("roughness", NodeMaterialBlockConnectionPointTypes::Float, true,
                            NodeMaterialBlockTargets::Fragment);

  sheenBlock->registerOutput(
    "sheen", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "sheen", sheenBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> SheenBlockPtr { return SheenBlock::New(iName); },
      "SheenBlock"));
}

void SheenBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("sheenOut");
  state._excludeVariableName("sheenMapData");
  state._excludeVariableName("vSheenColor");
  state._excludeVariableName("vSheenRoughness");
}

std::string SheenBlock::getClassName() const
{
  return "SheenBlock";
}

NodeMaterialConnectionPointPtr& SheenBlock::get_intensity()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& SheenBlock::get_color()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& SheenBlock::get_roughness()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& SheenBlock::get_sheen()
{
  return _outputs[0];
}

void SheenBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                NodeMaterialDefines& defines, bool /*useInstances*/,
                                SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  defines.setValue("SHEEN", true);
  defines.setValue("SHEEN_USE_ROUGHNESS_FROM_MAINTEXTURE", true, true);
  defines.setValue("SHEEN_LINKWITHALBEDO", linkSheenWithAlbedo, true);
  defines.setValue("SHEEN_ROUGHNESS", roughness()->isConnected(), true);
  defines.setValue("SHEEN_ALBEDOSCALING", albedoScaling, true);
}

std::string SheenBlock::getCode(const ReflectionBlockPtr& reflectionBlock) const
{
  const auto iColor     = color()->isConnected() ? color()->associatedVariableName() : "vec3(1.)";
  const auto iIntensity = intensity()->isConnected() ? intensity()->associatedVariableName() : "1.";
  const auto iRoughness = roughness()->isConnected() ? roughness()->associatedVariableName() : "0.";
  const auto iTexture   = "vec4(0.)";

  const auto code = StringTools::printf(
    R"(#ifdef SHEEN
        sheenOutParams sheenOut;

        vec4 vSheenColor = vec4(%s, %s);

        sheenBlock(
            vSheenColor,
        #ifdef SHEEN_ROUGHNESS
            %s,
        #endif
            roughness,
        #ifdef SHEEN_TEXTURE
            %s,
        #endif
            reflectance,
        #ifdef SHEEN_LINKWITHALBEDO
            baseColor,
            surfaceAlbedo,
        #endif
        #ifdef ENVIRONMENTBRDF
            NdotV,
            environmentBrdf,
        #endif
        #if defined(REFLECTION) && defined(ENVIRONMENTBRDF)
            AARoughnessFactors,
            %s,
            %s,
            %s,
            vLightingIntensity,
            #ifdef %s
                %s,
            #else
                %s,
            #endif
            reflectionOut.reflectionCoords,
            NdotVUnclamped,
            #ifndef LODBASEDMICROSFURACE
                #ifdef %s
                    %s,
                    %s,
                #else
                    %s,
                    %s,
                #endif
            #endif
            #if !defined(%s) && defined(RADIANCEOCCLUSION)
                seo,
            #endif
            #if !defined(%s) && defined(HORIZONOCCLUSION) && defined(BUMP) && defined(%s)
                eho,
            #endif
        #endif
            sheenOut
        );

        #ifdef SHEEN_LINKWITHALBEDO
            surfaceAlbedo = sheenOut.surfaceAlbedo;
        #endif
    #endif\r\n
    )",
    iColor.c_str(), iIntensity.c_str(),                                                //
    iRoughness.c_str(),                                                                //
    iTexture,                                                                          //
    reflectionBlock->_vReflectionMicrosurfaceInfosName.c_str(),                        //
    reflectionBlock->_vReflectionInfosName.c_str(),                                    //
    reflectionBlock->reflectionColor().c_str(),                                        //
    reflectionBlock->_define3DName.c_str(),                                            //
    reflectionBlock->_cubeSamplerName.c_str(),                                         //
    reflectionBlock->_2DSamplerName.c_str(),                                           //
    reflectionBlock->_define3DName.c_str(),                                            //
    reflectionBlock->_cubeSamplerName.c_str(),                                         //
    reflectionBlock->_cubeSamplerName.c_str(),                                         //
    reflectionBlock->_2DSamplerName.c_str(),                                           //
    reflectionBlock->_2DSamplerName.c_str(),                                           //
    reflectionBlock->_defineSkyboxName.c_str(),                                        //
    reflectionBlock->_defineSkyboxName.c_str(), reflectionBlock->_define3DName.c_str() //
  );

  return code;
}

SheenBlock& SheenBlock::_buildBlock(NodeMaterialBuildState& state)
{
  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  return *this;
}

std::string SheenBlock::_dumpPropertiesCode()
{
  auto codeString = NodeMaterialBlock::_dumpPropertiesCode();

  codeString += StringTools::printf("%s.albedoScaling = %s;\r\n", _codeVariableName.c_str(),
                                    albedoScaling ? "true" : "false");
  codeString += StringTools::printf("%s.linkSheenWithAlbedo = %s;\r\n", _codeVariableName.c_str(),
                                    linkSheenWithAlbedo ? "true" : "false");

  return codeString;
}

json SheenBlock::serialize() const
{
  return nullptr;
}

void SheenBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
