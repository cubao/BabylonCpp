#include <babylon/materials/textures/base_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

BaseTexture::BaseTexture(Scene* scene)
    : getAlphaFromRGB{false}
    , level{1.f}
    , coordinatesIndex{0}
    , wrapU{TextureConstants::WRAP_ADDRESSMODE}
    , wrapV{TextureConstants::WRAP_ADDRESSMODE}
    , wrapR{TextureConstants::WRAP_ADDRESSMODE}
    , anisotropicFilteringLevel{BaseTexture::
                                  DEFAULT_ANISOTROPIC_FILTERING_LEVEL}
    , isCube{false}
    , is3D{false}
    , gammaSpace{true}
    , isRGBD{this, &BaseTexture::get_isRGBD}
    , invertZ{false}
    , lodLevelInAlpha{false}
    , lodGenerationOffset{0.f}
    , lodGenerationScale{0.8f}
    , isRenderTarget{false}
    , delayLoadState{EngineConstants::DELAYLOADSTATE_NONE}
    , _texture{nullptr}
    , boundingBoxSize{this, &BaseTexture::get_boundingBoxSize,
                      &BaseTexture::set_boundingBoxSize}
    , _hasAlpha{false}
    , _coordinatesMode{TextureConstants::EXPLICIT_MODE}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _onDisposeObserver{nullptr}
    , _textureMatrix{Matrix::IdentityReadOnly()}
    , _reflectionTextureMatrix{Matrix::IdentityReadOnly()}
    , emptyVector3{nullptr}
{
}

BaseTexture::~BaseTexture()
{
}

IReflect::Type BaseTexture::type() const
{
  return IReflect::Type::BASETEXTURE;
}

void BaseTexture::addToScene(unique_ptr_t<BaseTexture>&& newTexture)
{
  if (_scene) {
    _scene->textures.emplace_back(::std::move(newTexture));
  }
}

void BaseTexture::setHasAlpha(bool value)
{
  if (_hasAlpha == value) {
    return;
  }
  _hasAlpha = value;
  if (_scene) {
    _scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag()
                                    | Material::MiscDirtyFlag());
  }
}

bool BaseTexture::hasAlpha() const
{
  return _hasAlpha;
}

void BaseTexture::setCoordinatesMode(unsigned int value)
{
  if (_coordinatesMode == value) {
    return;
  }
  _coordinatesMode = value;
  if (_scene) {
    _scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag());
  }
}

unsigned int BaseTexture::coordinatesMode() const
{
  return _coordinatesMode;
}

bool BaseTexture::get_isRGBD() const
{
  return _texture != nullptr && _texture->_isRGBD;
}

string_t BaseTexture::uid()
{
  if (_uid.empty()) {
    _uid = Tools::RandomId();
  }
  return _uid;
}

string_t BaseTexture::toString() const
{
  return name;
}

const string_t BaseTexture::getClassName() const
{
  return "BaseTexture";
}

void BaseTexture::setOnDispose(
  const ::std::function<void(BaseTexture*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

bool BaseTexture::isBlocking() const
{
  return true;
}

Scene* BaseTexture::getScene() const
{
  return _scene;
}

Matrix* BaseTexture::getTextureMatrix()
{
  return &_textureMatrix;
}

Matrix* BaseTexture::getReflectionTextureMatrix()
{
  return &_reflectionTextureMatrix;
}

InternalTexture* BaseTexture::getInternalTexture()
{
  return _texture;
}

bool BaseTexture::isReadyOrNotBlocking()
{
  return !isBlocking() || isReady();
}

bool BaseTexture::isReady()
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    delayLoad();
    return false;
  }

  if (_texture) {
    return _texture->isReady;
  }

  return false;
}

ISize BaseTexture::getSize() const
{
  if (_texture && _texture->width) {
    return Size(_texture->width, _texture->height);
  }

  if (_texture && _texture->_size) {
    return Size(_texture->_size, _texture->_size);
  }

  return Size::Zero();
}

ISize BaseTexture::getBaseSize()
{
  if (!isReady() || !_texture) {
    return Size::Zero();
  }

  if (_texture->_size) {
    return Size(_texture->_size, _texture->_size);
  }

  return Size(_texture->baseWidth, _texture->baseHeight);
}

void BaseTexture::scale(float /*ratio*/)
{
}

bool BaseTexture::canRescale()
{
  return false;
}

InternalTexture* BaseTexture::_getFromCache(const string_t& url, bool noMipmap,
                                            unsigned int sampling)
{
  if (!_scene) {
    return nullptr;
  }

  auto& texturesCache = _scene->getEngine()->getLoadedTexturesCache();
  for (auto& texturesCacheEntry : texturesCache) {
    if ((texturesCacheEntry->url.compare(url) == 0)
        && texturesCacheEntry->generateMipMaps != noMipmap) {
      if (!sampling || sampling == texturesCacheEntry->samplingMode) {
        texturesCacheEntry->incrementReferences();
        return texturesCacheEntry.get();
      }
    }
  }

  return nullptr;
}

void BaseTexture::_rebuild()
{
}

void BaseTexture::delayLoad()
{
}

void BaseTexture::set_boundingBoxSize(const Nullable<Vector3>& /*value*/)
{
}

Nullable<Vector3>& BaseTexture::get_boundingBoxSize()
{
  return emptyVector3;
}

vector_t<Animation*> BaseTexture::getAnimations()
{
  return animations;
}

unique_ptr_t<BaseTexture> BaseTexture::clone() const
{
  return nullptr;
}

unsigned int BaseTexture::textureType() const
{
  if (!_texture) {
    return EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  }

  return _texture->type ? _texture->type :
                          EngineConstants::TEXTURETYPE_UNSIGNED_INT;
}

unsigned int BaseTexture::textureFormat() const
{
  if (!_texture) {
    return EngineConstants::TEXTUREFORMAT_RGBA;
  }

  return _texture->format ? _texture->format :
                            EngineConstants::TEXTUREFORMAT_RGBA;
}

ArrayBufferView BaseTexture::readPixels(unsigned int faceIndex)
{
  if (!_texture) {
    return ArrayBufferView();
  }

  auto size  = getSize();
  auto scene = getScene();

  if (!scene) {
    return ArrayBufferView();
  }

  auto engine = scene->getEngine();

  if (_texture->isCube) {
    return engine->_readTexturePixels(_texture, size.width, size.height,
                                      static_cast<int>(faceIndex));
  }

  return engine->_readTexturePixels(_texture, size.width, size.height, -1);
}

void BaseTexture::releaseInternalTexture()
{
  if (_texture) {
    _texture->dispose();
    _texture = nullptr;
  }
}

SphericalPolynomial* BaseTexture::sphericalPolynomial()
{
  if (!_texture || !isReady()) {
    return nullptr;
  }

  if (!_texture->_sphericalPolynomial) {
    _texture->_sphericalPolynomial = CubeMapToSphericalPolynomialTools::
      ConvertCubeMapTextureToSphericalPolynomial(this);
  }

  return _texture->_sphericalPolynomial.get();
}

void BaseTexture::setSphericalPolynomial(const SphericalPolynomial& value)
{
  if (_texture) {
    _texture->_sphericalPolynomial
      = ::std::make_unique<SphericalPolynomial>(value);
  }
}

BaseTexture* BaseTexture::_lodTextureHigh() const
{
  if (_texture) {
    return _texture->_lodTextureHigh;
  }
  return nullptr;
}

BaseTexture* BaseTexture::_lodTextureMid() const
{
  if (_texture) {
    return _texture->_lodTextureMid;
  }
  return nullptr;
}

BaseTexture* BaseTexture::_lodTextureLow() const
{
  if (_texture) {
    return _texture->_lodTextureLow;
  }
  return nullptr;
}

void BaseTexture::dispose()
{
  if (!_scene) {
    return;
  }

  // Animations
  _scene->stopAnimation(this);

  // Remove from scene
  _scene->textures.erase(
    ::std::remove_if(_scene->textures.begin(), _scene->textures.end(),
                     [this](const unique_ptr_t<BaseTexture>& baseTexture) {
                       return baseTexture.get() == this;
                     }),
    _scene->textures.end());

  if (_texture == nullptr) {
    return;
  }

  // Release
  releaseInternalTexture();

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

Json::object BaseTexture::serialize() const
{
  return Json::object();
}

void BaseTexture::WhenAllReady(const vector_t<BaseTexture*>& textures,
                               const ::std::function<void()>& callback)
{
  auto numRemaining = textures.size();
  if (numRemaining == 0) {
    callback();
    return;
  }

  for (auto& texture : textures) {
    if (texture->isReady()) {
      if (--numRemaining == 0) {
        callback();
      }
    }
    else {
      auto onLoadObservable
        = *(static_cast<Texture*>(texture))->onLoadObservable();

      const ::std::function<void(Texture*, EventState&)> onLoadCallback
        = [&](Texture*, EventState&) {
            onLoadObservable.removeCallback(onLoadCallback);
            if (--numRemaining == 0) {
              callback();
            }
          };

      onLoadObservable.add(onLoadCallback);
    }
  }
}

} // end of namespace BABYLON
