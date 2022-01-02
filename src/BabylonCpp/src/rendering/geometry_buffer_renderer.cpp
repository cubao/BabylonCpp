#include <babylon/rendering/geometry_buffer_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/maths/size.h>
#include <babylon/meshes/_instance_data_storage.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/rendering/geometry_buffer_renderer_scene_component.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

GeometryBufferRenderer::GeometryBufferRenderer(Scene* scene, float ratio)
    : renderTransparentMeshes{true}
    , renderList{this, &GeometryBufferRenderer::get_renderList,
                 &GeometryBufferRenderer::set_renderList}
    , isSupported{this, &GeometryBufferRenderer::get_isSupported}
    , enablePosition{this, &GeometryBufferRenderer::get_enablePosition,
                     &GeometryBufferRenderer::set_enablePosition}
    , enableVelocity{this, &GeometryBufferRenderer::get_enableVelocity,
                     &GeometryBufferRenderer::set_enableVelocity}
    , enableReflectivity{this, &GeometryBufferRenderer::get_enableReflectivity,
                         &GeometryBufferRenderer::set_enableReflectivity}
    , scene{this, &GeometryBufferRenderer::get_scene}
    , ratio{this, &GeometryBufferRenderer::get_ratio}
    , samples{this, &GeometryBufferRenderer::get_samples, &GeometryBufferRenderer::set_samples}
    , _effect{nullptr}
    , _cachedDefines{""}
    , _resizeObserver{nullptr}
    , _multiRenderTarget{nullptr}
    , _enablePosition{false}
    , _enableVelocity{false}
    , _enableReflectivity{false}
    , _positionIndex{-1}
    , _velocityIndex{-1}
    , _reflectivityIndex{-1}
    , _depthIndex{-1}
    , _normalIndex{-1}
    , _depthNormalIndex{-1}
    , _linkedWithPrePass{false}
    , _prePassRenderer{nullptr}
{
  _scene = scene;
  _ratio = ratio;

  // Register the G Buffer component to the scene.
  auto component = std::static_pointer_cast<GeometryBufferRendererSceneComponent>(
    scene->_getComponent(SceneComponentConstants::NAME_GEOMETRYBUFFERRENDERER));
  if (!component) {
    component = GeometryBufferRendererSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  // Render target
  _createRenderTargets();
}

GeometryBufferRenderer::~GeometryBufferRenderer() = default;

void GeometryBufferRenderer::_linkPrePassRenderer(const PrePassRendererPtr& prePassRenderer)
{
  _linkedWithPrePass = true;
  _prePassRenderer   = prePassRenderer;

  if (_multiRenderTarget) {
    // prevents clearing of the RT since it's done by prepass
    _multiRenderTarget->onClearObservable.clear();
    _multiRenderTarget->onClearObservable.add([](Engine* /*engine*/, EventState& /*es*/) -> void {
      // pass
    });
  }
}

void GeometryBufferRenderer::_unlinkPrePassRenderer()
{
  _linkedWithPrePass = false;
  _createRenderTargets();
}

void GeometryBufferRenderer::_resetLayout()
{
  _enablePosition     = false;
  _enableReflectivity = false;
  _enableVelocity     = false;
  _attachments        = {};
}

void GeometryBufferRenderer::_forceTextureType(unsigned int geometryBufferType, int index)
{
  if (geometryBufferType == GeometryBufferRenderer::POSITION_TEXTURE_TYPE) {
    _positionIndex  = index;
    _enablePosition = true;
  }
  else if (geometryBufferType == GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE) {
    _velocityIndex  = index;
    _enableVelocity = true;
  }
  else if (geometryBufferType == GeometryBufferRenderer::REFLECTIVITY_TEXTURE_TYPE) {
    _reflectivityIndex  = index;
    _enableReflectivity = true;
  }
  else if (geometryBufferType == GeometryBufferRenderer::DEPTH_TEXTURE_TYPE) {
    _depthIndex = index;
  }
  else if (geometryBufferType == GeometryBufferRenderer::NORMAL_TEXTURE_TYPE) {
    _normalIndex = index;
  }
}

void GeometryBufferRenderer::_setAttachments(const std::vector<unsigned int>& attachments)
{
  _attachments = attachments;
}

void GeometryBufferRenderer::_linkInternalTexture(const InternalTexturePtr& internalTexture)
{
  _multiRenderTarget->_texture = internalTexture;
}

std::vector<AbstractMesh*>& GeometryBufferRenderer::get_renderList()
{
  return _multiRenderTarget->renderList();
}

void GeometryBufferRenderer::set_renderList(const std::vector<AbstractMesh*>& meshes)
{
  _multiRenderTarget->renderList = meshes;
}

bool GeometryBufferRenderer::get_isSupported() const
{
  return _multiRenderTarget->isSupported();
}

int GeometryBufferRenderer::getTextureIndex(unsigned int textureType)
{
  switch (textureType) {
    case GeometryBufferRenderer::POSITION_TEXTURE_TYPE:
      return _positionIndex;
    case GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE:
      return _velocityIndex;
    case GeometryBufferRenderer::REFLECTIVITY_TEXTURE_TYPE:
      return _reflectivityIndex;
    default:
      return -1;
  }
}

bool GeometryBufferRenderer::get_enablePosition() const
{
  return _enablePosition;
}

void GeometryBufferRenderer::set_enablePosition(bool enable)
{
  _enablePosition = enable;

  // PrePass handles index and texture links
  if (!_linkedWithPrePass) {
    dispose();
    _createRenderTargets();
  }
}

bool GeometryBufferRenderer::get_enableVelocity() const
{
  return _enableVelocity;
}

void GeometryBufferRenderer::set_enableVelocity(bool enable)
{
  _enableVelocity = enable;

  if (!enable) {
    _previousTransformationMatrices = {};
  }

  if (!_linkedWithPrePass) {
    dispose();
    _createRenderTargets();
  }
}

bool GeometryBufferRenderer::get_enableReflectivity() const
{
  return _enableReflectivity;
}

void GeometryBufferRenderer::set_enableReflectivity(bool enable)
{
  _enableReflectivity = enable;

  if (!_linkedWithPrePass) {
    dispose();
    _createRenderTargets();
  }
}

Scene*& GeometryBufferRenderer::get_scene()
{
  return _scene;
}

float GeometryBufferRenderer::get_ratio() const
{
  return _ratio;
}

bool GeometryBufferRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();

  if (material && material->disableDepthWrite) {
    return false;
  }

  std::vector<std::string> defines;

  std::vector<std::string> attribs{VertexBuffer::PositionKind, VertexBuffer::NormalKind};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material) {
    auto needUv = false;
    if (material->needAlphaTesting()) {
      defines.emplace_back("#define ALPHATEST");
      needUv = true;
    }

    auto asStandardMaterial = std::static_pointer_cast<StandardMaterial>(material);
    auto asPBRMaterial      = std::static_pointer_cast<PBRMaterial>(material);

    if (((asStandardMaterial && asStandardMaterial->bumpTexture())
         || (asPBRMaterial && asPBRMaterial->bumpTexture()))
        && StandardMaterial::BumpTextureEnabled()) {
      defines.emplace_back("#define BUMP");
      defines.emplace_back("#define BUMPDIRECTUV 0");
      needUv = true;
    }

    if (_enableReflectivity) {
      if (asStandardMaterial && asStandardMaterial->specularTexture()) {
        defines.emplace_back("#define HAS_SPECULAR");
        needUv = true;
      }
      else if (asPBRMaterial && asPBRMaterial->reflectivityTexture()) {
        defines.emplace_back("#define HAS_REFLECTIVITY");
        needUv = true;
      }
    }

    if (needUv) {
      defines.emplace_back("#define NEED_UV");
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        attribs.emplace_back(VertexBuffer::UVKind);
        defines.emplace_back("#define UV1");
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        attribs.emplace_back(VertexBuffer::UV2Kind);
        defines.emplace_back("#define UV2");
      }
    }
  }

  // PrePass
  if (_linkedWithPrePass) {
    defines.emplace_back("#define PREPASS");
    if (_depthNormalIndex != -1) {
      defines.emplace_back("#define DEPTHNORMAL_INDEX " + std::to_string(_depthNormalIndex));
      defines.emplace_back("#define PREPASS_DEPTHNORMAL");
    }
  }

  // Buffers
  if (_enablePosition) {
    defines.emplace_back("#define POSITION");
    defines.emplace_back("#define POSITION_INDEX " + std::to_string(_positionIndex));
  }

  if (_enableVelocity) {
    defines.emplace_back("#define VELOCITY");
    defines.emplace_back("#define VELOCITY_INDEX " + std::to_string(_velocityIndex));
    if (stl_util::contains(excludedSkinnedMeshesFromVelocity, mesh)) {
      defines.emplace_back("#define BONES_VELOCITY_ENABLED");
    }
  }

  if (_enableReflectivity) {
    defines.emplace_back("#define REFLECTIVITY");
    defines.emplace_back("#define REFLECTIVITY_INDEX " + std::to_string(_reflectivityIndex));
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      "#define BonesPerMesh "
      + std::to_string(mesh->skeleton() ? mesh->skeleton()->bones.size() + 1 : 0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Morph targets
  auto morphTargetManager  = std::static_pointer_cast<Mesh>(mesh)->morphTargetManager();
  auto numMorphInfluencers = 0ull;
  if (morphTargetManager) {
    if (morphTargetManager->numInfluencers() > 0) {
      numMorphInfluencers = morphTargetManager->numInfluencers();

      defines.emplace_back("#define MORPHTARGETS");
      defines.emplace_back("#define NUM_MORPH_INFLUENCERS " + std::to_string(numMorphInfluencers));

      MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(
        attribs, mesh.get(), static_cast<unsigned int>(numMorphInfluencers));
    }
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
    if (subMesh->getRenderingMesh()->hasThinInstances()) {
      defines.emplace_back("#define THIN_INSTANCES");
    }
  }

  // Setup textures count
  if (_linkedWithPrePass) {
    defines.emplace_back("#define RENDER_TARGET_COUNT " + std::to_string(_attachments.size()));
  }
  else {
    defines.emplace_back("#define RENDER_TARGET_COUNT "
                         + std::to_string(_multiRenderTarget->textures().size()));
  }

  // Get correct effect
  auto join = StringTools::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    const std::unordered_map<std::string, unsigned int> indexParameters{
      {"buffersCount", _enablePosition ? 3u : 2u}};

    IEffectCreationOptions options;
    options.attributes      = std::move(attribs);
    options.uniformsNames   = {"world",
                             "mBones",
                             "viewProjection",
                             "diffuseMatrix",
                             "view",
                             "previousWorld",
                             "previousViewProjection",
                             "mPreviousBones",
                             "morphTargetInfluences",
                             "bumpMatrix",
                             "reflectivityMatrix",
                             "vTangentSpaceParams",
                             "vBumpInfos"};
    options.samplers        = {"diffuseSampler", "bumpSampler", "reflectivitySampler"};
    options.defines         = std::move(join);
    options.indexParameters = std::move(indexParameters);
    options.indexParameters
      = {{"buffersCount", static_cast<unsigned>(_multiRenderTarget->textures().size() - 1)},
         {"maxSimultaneousMorphTargets", static_cast<unsigned>(numMorphInfluencers)}};

    _effect = _scene->getEngine()->createEffect("geometry", options, _scene->getEngine());
  }

  return _effect->isReady();
}

MultiRenderTargetPtr GeometryBufferRenderer::getGBuffer() const
{
  return _multiRenderTarget;
}

unsigned int GeometryBufferRenderer::get_samples() const
{
  return _multiRenderTarget->samples();
}

void GeometryBufferRenderer::set_samples(unsigned int value)
{
  _multiRenderTarget->samples = value;
}

void GeometryBufferRenderer::dispose()
{
  if (_resizeObserver) {
    const auto engine = _scene->getEngine();
    engine->onResizeObservable.remove(_resizeObserver);
    _resizeObserver = nullptr;
  }
  getGBuffer()->dispose();
}

int GeometryBufferRenderer::_assignRenderTargetIndices()
{
  auto count = 2;

  if (_enablePosition) {
    _positionIndex = count;
    ++count;
  }

  if (_enableVelocity) {
    _velocityIndex = count;
    ++count;
  }

  if (_enableReflectivity) {
    _reflectivityIndex = count;
    ++count;
  }

  return count;
}

void GeometryBufferRenderer::_createRenderTargets()
{
  const auto engine = _scene->getEngine();
  const auto count  = _assignRenderTargetIndices();

  // Render target
  IMultiRenderTargetOptions options;
  options.generateMipMaps      = false;
  options.generateDepthTexture = true;
  options.defaultType          = Constants::TEXTURETYPE_FLOAT;
  _multiRenderTarget           = std::make_shared<MultiRenderTarget>(
    "gBuffer",
    RenderTargetSize{static_cast<int>(engine->getRenderWidth() * _ratio),
                     static_cast<int>(engine->getRenderHeight() * _ratio)},
    count, _scene, options);
  if (!isSupported()) {
    return;
  }

  _multiRenderTarget->wrapU           = TextureConstants::CLAMP_ADDRESSMODE;
  _multiRenderTarget->wrapV           = TextureConstants::CLAMP_ADDRESSMODE;
  _multiRenderTarget->refreshRate     = 1;
  _multiRenderTarget->renderParticles = false;
  _multiRenderTarget->renderList      = {};

  // set default depth value to 1.0 (far away)
  _multiRenderTarget->onClearObservable.add([](Engine* engine, EventState&) {
    engine->clear(Color4(0.f, 0.f, 0.f, 1.f), true, true, true);
  });

  _resizeObserver
    = engine->onResizeObservable.add([this](Engine* engine, EventState& /*es*/) -> void {
        if (_multiRenderTarget) {
          _multiRenderTarget->resize(Size{
            static_cast<int>(engine->getRenderWidth() * _ratio), // width,
            static_cast<int>(engine->getRenderHeight() * _ratio) // height
          });
        }
      });

  // Custom render function
  _multiRenderTarget->customRenderFunction
    = [this, engine](const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& depthOnlySubMeshes,
                     const std::function<void()>& /*beforeTransparents*/) {
        if (_linkedWithPrePass) {
          if (!_prePassRenderer->enabled()) {
            return;
          }
          _scene->getEngine()->bindAttachments(_attachments);
        }

        if (!depthOnlySubMeshes.empty()) {
          engine->setColorWrite(false);
          for (const auto& depthOnlySubMesh : depthOnlySubMeshes) {
            renderSubMesh(depthOnlySubMesh);
          }
          engine->setColorWrite(true);
        }

        for (const auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }

        if (renderTransparentMeshes) {
          for (const auto& transparentSubMesh : transparentSubMeshes) {
            renderSubMesh(transparentSubMesh);
          }
        }
      };
}

void GeometryBufferRenderer::renderSubMesh(SubMesh* subMesh)
{
  auto renderingMesh = subMesh->getRenderingMesh();
  auto effectiveMesh = subMesh->getEffectiveMesh();
  auto engine        = _scene->getEngine();
  auto material      = subMesh->getMaterial();

  if (!material) {
    return;
  }

  effectiveMesh->_internalAbstractMeshDataInfo._isActiveIntermediate = false;

  // Velocity
  if (_enableVelocity
      && !stl_util::contains(_previousTransformationMatrices, effectiveMesh->uniqueId)) {
    _previousTransformationMatrices[effectiveMesh->uniqueId] = ISavedTransformationMatrix{
      Matrix::Identity(),          // world
      _scene->getTransformMatrix() // viewProjection
    };

    if (renderingMesh->skeleton()) {
      const auto bonesTransformations
        = renderingMesh->skeleton()->getTransformMatrices(renderingMesh.get());
      Float32Array dest(bonesTransformations.size());
      _previousBonesTransformationMatrices[renderingMesh->uniqueId]
        = _copyBonesTransformationMatrices(bonesTransformations, dest);
    }
  }

  // Managing instances
  auto batch = renderingMesh->_getInstancesRenderList(subMesh->_id,
                                                      subMesh->getReplacementMesh() != nullptr);

  if (batch->mustReturn) {
    return;
  }

  auto hardwareInstancedRendering = ((engine->getCaps().instancedArrays != 0)
                                     && (stl_util::contains(batch->visibleInstances, subMesh->_id))
                                     && (!batch->visibleInstances[subMesh->_id].empty()))
                                    || renderingMesh->hasThinInstances();
  auto world = effectiveMesh->getWorldMatrix();

  if (isReady(subMesh, hardwareInstancedRendering)) {
    engine->enableEffect(_effect);
    renderingMesh->_bind(subMesh, _effect, material->fillMode());

    _effect->setMatrix("viewProjection", _scene->getTransformMatrix());
    _effect->setMatrix("view", _scene->getViewMatrix());

    if (material) {
      std::optional<unsigned int> sideOrientation = std::nullopt;
      const auto effectiveMeshCasted              = std::static_pointer_cast<Mesh>(effectiveMesh);
      const auto& instanceDataStorage             = effectiveMeshCasted->_instanceDataStorage;

      if (!instanceDataStorage->isFrozen
          && (material->backFaceCulling
              || effectiveMeshCasted->overrideMaterialSideOrientation.has_value())) {
        const auto mainDeterminant = effectiveMesh->_getWorldMatrixDeterminant();
        sideOrientation            = effectiveMeshCasted->overrideMaterialSideOrientation;
        if (!sideOrientation.has_value()) {
          sideOrientation = material->sideOrientation;
        }
        if (mainDeterminant < 0.f) {
          sideOrientation = (sideOrientation == Material::ClockWiseSideOrientation ?
                               Material::CounterClockWiseSideOrientation :
                               Material::ClockWiseSideOrientation);
        }
      }
      else {
        sideOrientation = instanceDataStorage->sideOrientation;
      }

      material->_preBind(_effect, sideOrientation);

      // Alpha test
      if (material->needAlphaTesting()) {
        auto alphaTexture = material->getAlphaTestTexture();
        if (alphaTexture) {
          _effect->setTexture("diffuseSampler", alphaTexture);
          _effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
        }
      }

      auto asStandardMaterial = std::static_pointer_cast<StandardMaterial>(material);
      auto asPBRMaterial      = std::static_pointer_cast<PBRMaterial>(material);

      // Bump
      if (scene()->getEngine()->getCaps().standardDerivatives
          && StandardMaterial::BumpTextureEnabled()) {
        if (asStandardMaterial) {
          if (asStandardMaterial->bumpTexture()) {
            _effect->setFloat3(
              "vBumpInfos", static_cast<float>(asStandardMaterial->bumpTexture()->coordinatesIndex),
              1.f / asStandardMaterial->bumpTexture()->level,
              asStandardMaterial->parallaxScaleBias);
            _effect->setMatrix("bumpMatrix",
                               *asStandardMaterial->bumpTexture()->getTextureMatrix());
            _effect->setTexture("bumpSampler", asStandardMaterial->bumpTexture());
            _effect->setFloat2("vTangentSpaceParams",
                               asStandardMaterial->invertNormalMapX() ? -1.f : 1.f,
                               asStandardMaterial->invertNormalMapY ? -1.f : 1.f);
          }
        }

        if (asPBRMaterial) {
          if (asPBRMaterial->bumpTexture()) {
            _effect->setFloat3(
              "vBumpInfos", static_cast<float>(asPBRMaterial->bumpTexture()->coordinatesIndex),
              1.f / asPBRMaterial->bumpTexture()->level, asPBRMaterial->parallaxScaleBias);
            _effect->setMatrix("bumpMatrix", *asPBRMaterial->bumpTexture()->getTextureMatrix());
            _effect->setTexture("bumpSampler", asPBRMaterial->bumpTexture());
            _effect->setFloat2("vTangentSpaceParams",
                               asPBRMaterial->invertNormalMapX() ? -1.f : 1.f,
                               asPBRMaterial->invertNormalMapY ? -1.f : 1.f);
          }
        }
      }

      // Roughness
      if (_enableReflectivity) {
        if (asStandardMaterial && asStandardMaterial->specularTexture()) {
          _effect->setMatrix("reflectivityMatrix",
                             *asStandardMaterial->specularTexture()->getTextureMatrix());
          _effect->setTexture("reflectivitySampler", asStandardMaterial->specularTexture());
        }
        else if (asPBRMaterial && asPBRMaterial->reflectivityTexture()) {
          _effect->setMatrix("reflectivityMatrix",
                             *asPBRMaterial->reflectivityTexture()->getTextureMatrix());
          _effect->setTexture("reflectivitySampler", asPBRMaterial->reflectivityTexture());
        }
      }
    }

    // Bones
    if (renderingMesh->useBones() && renderingMesh->computeBonesUsingShaders()
        && renderingMesh->skeleton()) {
      _effect->setMatrices("mBones",
                           renderingMesh->skeleton()->getTransformMatrices(renderingMesh.get()));
      if (_enableVelocity) {
        _effect->setMatrices("mPreviousBones",
                             _previousBonesTransformationMatrices[renderingMesh->uniqueId]);
      }
    }

    // Morph targets
    MaterialHelper::BindMorphTargetParameters(renderingMesh.get(), _effect.get());

    // Velocity
    if (_enableVelocity) {
      _effect->setMatrix("previousWorld",
                         _previousTransformationMatrices[effectiveMesh->uniqueId].world);
      _effect->setMatrix("previousViewProjection",
                         _previousTransformationMatrices[effectiveMesh->uniqueId].viewProjection);
    }

    // Draw
    renderingMesh->_processRendering(
      effectiveMesh.get(), subMesh, _effect, static_cast<int>(material->fillMode()), batch,
      hardwareInstancedRendering,
      [this](bool /*isInstance*/, Matrix world, Material* /*effectiveMaterial*/) {
        _effect->setMatrix("world", world);
      });
  }

  // Velocity
  if (_enableVelocity) {
    _previousTransformationMatrices[effectiveMesh->uniqueId].world = world;
    _previousTransformationMatrices[effectiveMesh->uniqueId].viewProjection
      = _scene->getTransformMatrix();
    if (renderingMesh->skeleton()) {
      _copyBonesTransformationMatrices(
        renderingMesh->skeleton()->getTransformMatrices(renderingMesh.get()),
        _previousBonesTransformationMatrices[effectiveMesh->uniqueId]);
    }
  }
}

Float32Array& GeometryBufferRenderer::_copyBonesTransformationMatrices(const Float32Array& source,
                                                                       Float32Array& target)
{
  for (size_t i = 0; i < source.size(); ++i) {
    target[i] = source[i];
  }

  return target;
}

} // end of namespace BABYLON
