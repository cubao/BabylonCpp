#include <babylon/misc/environment_texture_tools.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/spherical_polynomial.h>
#include <babylon/maths/vector3.h>
#include <babylon/misc/environment_texture_info.h>
#include <babylon/misc/environment_texture_irradiance_info_v1.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

std::array<uint8_t, 8> EnvironmentTextureTools::_MagicBytes
  = {0x86, 0x16, 0x87, 0x96, 0xf6, 0xd6, 0x96, 0x36};

EnvironmentTextureTools::EnvironmentTextureTools() = default;

EnvironmentTextureTools::~EnvironmentTextureTools() = default;

EnvironmentTextureInfoPtr EnvironmentTextureTools::GetEnvInfo(const ArrayBufferView& data)
{
  const auto& dataView
    = stl_util::to_array<int32_t>(data.buffer(), data.byteOffset, data.byteLength());
  auto pos = 0ull;

  for (unsigned char magicByte : EnvironmentTextureTools::_MagicBytes) {
    if (dataView[pos++] != magicByte) {
      BABYLON_LOG_ERROR("EnvironmentTextureTools", "Not a babylon environment map")
      return nullptr;
    }
  }

  // Read json manifest - collect characters up to null terminator
  std::ostringstream manifestString;
  auto charCode = static_cast<char>(dataView[pos++]);
  while (charCode) {
    manifestString << StringTools::fromCharCode(charCode);
    charCode = static_cast<char>(dataView[pos++]);
  }

  // Parse JSON string
  auto parsedManifest = json::parse(manifestString.str());
  auto manifest       = EnvironmentTextureInfo::Parse(parsedManifest);
  if (manifest->specular) {
    // Extend the header with the position of the payload.
    manifest->specular->specularDataPosition = pos;
    // Fallback to 0.8 exactly if lodGenerationScale is not defined for backward
    // compatibility.
    manifest->specular->lodGenerationScale = manifest->specular->lodGenerationScale.value_or(0.8f);
  }

  return manifest;
}

EnvironmentTextureIrradianceInfoV1Ptr
EnvironmentTextureTools::_CreateEnvTextureIrradiance(const BaseTexturePtr& texture)
{
  auto polynmials = texture->sphericalPolynomial();
  if (polynmials == nullptr) {
    return nullptr;
  }

  auto info = std::make_shared<EnvironmentTextureIrradianceInfoV1>();

  info->x = {polynmials->x.x, polynmials->x.y, polynmials->x.z};
  info->y = {polynmials->y.x, polynmials->y.y, polynmials->y.z};
  info->z = {polynmials->z.x, polynmials->z.y, polynmials->z.z};

  info->xx = {polynmials->xx.x, polynmials->xx.y, polynmials->xx.z};
  info->yy = {polynmials->yy.x, polynmials->yy.y, polynmials->yy.z};
  info->zz = {polynmials->zz.x, polynmials->zz.y, polynmials->zz.z};

  info->yz = {polynmials->yz.x, polynmials->yz.y, polynmials->yz.z};
  info->zx = {polynmials->zx.x, polynmials->zx.y, polynmials->zx.z};
  info->xy = {polynmials->xy.x, polynmials->xy.y, polynmials->xy.z};

  return info;
}

std::vector<std::vector<ArrayBuffer>>
EnvironmentTextureTools::CreateImageDataArrayBufferViews(const ArrayBufferView& data,
                                                         const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    throw std::runtime_error("Unsupported babylon environment map version "
                             + std::to_string(info.version));
  }

  const auto& specularInfo = info.specular;

  // Double checks the enclosed info
  auto _mipmapsCount = Scalar::Log2(info.width);
  auto mipmapsCount  = static_cast<size_t>(std::round(_mipmapsCount) + 1);
  if (specularInfo->mipmaps.size() != 6 * mipmapsCount) {
    throw std::runtime_error("Unsupported specular mipmaps number "
                             + std::to_string(specularInfo->mipmaps.size()));
  }

  std::vector<std::vector<ArrayBuffer>> imageData(mipmapsCount);
  for (size_t i = 0; i < mipmapsCount; ++i) {
    imageData[i] = std::vector<ArrayBuffer>(6);
    for (size_t face = 0; face < 6; ++face) {
      auto imageInfo     = specularInfo->mipmaps[i * 6 + face];
      imageData[i][face] = stl_util::to_array<uint8_t>(
        data.buffer(), data.byteOffset + *specularInfo->specularDataPosition + imageInfo.position,
        imageInfo.length);
    }
  }

  return imageData;
}

void EnvironmentTextureTools::UploadEnvLevelsSync(const InternalTexturePtr& texture,
                                                  const ArrayBufferView& data,
                                                  const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    throw std::runtime_error("Unsupported babylon environment map version "
                             + std::to_string(info.version));
  }

  auto& specularInfo = info.specular;
  if (!specularInfo) {
    // Nothing else parsed so far
    return;
  }

  texture->_lodGenerationScale = *specularInfo->lodGenerationScale;

  const auto imageData = EnvironmentTextureTools::CreateImageDataArrayBufferViews(data, info);

  return EnvironmentTextureTools::UploadLevelsSync(texture, imageData);
}

void EnvironmentTextureTools::_OnImageReadySync(
  const Image& image, Engine* engine, bool expandTexture, const PostProcessPtr& rgbdPostProcess,
  const std::string& /*url*/, unsigned int face, int i, bool generateNonLODTextures,
  const std::unordered_map<size_t, BaseTexturePtr>& lodTextures, const InternalTexturePtr& cubeRtt,
  const InternalTexturePtr& texture)
{
  // Upload to the texture.
  if (expandTexture) {
    auto tempTexture = engine->createTexture(
      "", true, true, nullptr, Constants::TEXTURE_NEAREST_SAMPLINGMODE, nullptr,
      [](const std::string& message, const std::string& /*exception*/) {
        throw std::runtime_error(message);
      },
      image);

    rgbdPostProcess->getEffect()->executeWhenCompiled(
      [rgbdPostProcess, tempTexture, engine, cubeRtt, face, i](Effect* /*effect*/) {
        // Uncompress the data to a RTT
        rgbdPostProcess->onApply = [&](Effect* effect, EventState& /*es*/) {
          effect->_bindTexture("textureSampler", tempTexture);
          effect->setFloat2("scale", 1.f, 1.f);
        };

        engine->scenes[0]->postProcessManager->directRender({rgbdPostProcess}, cubeRtt, true, face,
                                                            static_cast<int>(i));

        // Cleanup
        engine->restoreDefaultFramebuffer();
        tempTexture->dispose();
      });
  }
  else {
    engine->_uploadImageToTexture(texture, image, face, static_cast<int>(i));

    // Upload the face to the non lod texture support
    if (generateNonLODTextures) {
      auto lodTexture = lodTextures.at(static_cast<size_t>(i));
      if (lodTexture) {
        engine->_uploadImageToTexture(lodTexture->_texture, image, face, 0);
      }
    }
  }
}

void EnvironmentTextureTools::UploadLevelsSync(
  const InternalTexturePtr& texture, const std::vector<std::vector<ArrayBuffer>>& imageData)
{
  if (!Tools::IsExponentOfTwo(static_cast<size_t>(texture->width))) {
    throw std::runtime_error("Texture size must be a power of two");
  }

  const auto mipmapsCount = static_cast<size_t>(std::round(Scalar::Log2(texture->width)) + 1);

  // Gets everything ready.
  auto engine                    = static_cast<Engine*>(texture->getEngine());
  auto expandTexture             = false;
  auto generateNonLODTextures    = false;
  PostProcessPtr rgbdPostProcess = nullptr;
  InternalTexturePtr cubeRtt     = nullptr;
  std::unordered_map<size_t, BaseTexturePtr> lodTextures;
  const auto& caps = engine->getCaps();

  texture->format          = Constants::TEXTUREFORMAT_RGBA;
  texture->type            = Constants::TEXTURETYPE_UNSIGNED_INT;
  texture->generateMipMaps = true;
  engine->updateTextureSamplingMode(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, texture);

  // Add extra process if texture lod is not supported
  if (!caps.textureLOD) {
    expandTexture          = false;
    generateNonLODTextures = true;
    lodTextures            = {};
  }
  // in webgl 1 there are no ways to either render or copy lod level information
  // for float textures.
  else if (engine->webGLVersion() < 2.f) {
    expandTexture = false;
  }
  // If half float available we can uncompress the texture
  else if (caps.textureHalfFloatRender && caps.textureHalfFloatLinearFiltering) {
    expandTexture = true;
    texture->type = Constants::TEXTURETYPE_HALF_FLOAT;
  }
  // If full float available we can uncompress the texture
  else if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
    expandTexture = true;
    texture->type = Constants::TEXTURETYPE_FLOAT;
  }

  // Expand the texture if possible
  if (expandTexture) {
    // Simply run through the decode PP
    rgbdPostProcess = PostProcess::New("rgbdDecode", "rgbdDecode", {}, {}, 1.f, nullptr,
                                       Constants::TEXTURE_TRILINEAR_SAMPLINGMODE, engine, false, "",
                                       texture->type, "", {}, false);

    texture->_isRGBD = false;
    texture->invertY = false;

    IRenderTargetOptions options;
    options.generateDepthBuffer   = false;
    options.generateMipMaps       = true;
    options.generateStencilBuffer = false;
    options.samplingMode          = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE;
    options.type                  = texture->type;
    options.format                = Constants::TEXTUREFORMAT_RGBA;

    cubeRtt = engine->createRenderTargetCubeTexture(texture->width, options);
  }
  else {
    texture->_isRGBD = true;
    texture->invertY = true;

    // In case of missing support, applies the same patch than DDS files.
    if (generateNonLODTextures) {
      auto mipSlices = 3u;
      auto scale     = texture->_lodGenerationScale;
      auto offset    = texture->_lodGenerationOffset;

      for (unsigned int i = 0; i < mipSlices; ++i) {
        // compute LOD from even spacing in smoothness (matching shader calculation)
        auto smoothness = static_cast<float>(i) / static_cast<float>(mipSlices - 1);
        auto roughness  = 1.f - smoothness;

        auto minLODIndex = offset; // roughness = 0
        auto maxLODIndex
          = (mipmapsCount - 1) * scale + offset; // roughness = 1 (mipmaps start from 0)

        auto lodIndex = minLODIndex + (maxLODIndex - minLODIndex) * roughness;
        auto mipmapIndex
          = static_cast<size_t>(std::round(std::min(std::max(lodIndex, 0.f), maxLODIndex)));

        auto glTextureFromLod     = InternalTexture::New(engine, InternalTextureSource::Temp);
        glTextureFromLod->isCube  = true;
        glTextureFromLod->invertY = true;
        glTextureFromLod->generateMipMaps = false;
        engine->updateTextureSamplingMode(Constants::TEXTURE_LINEAR_LINEAR, glTextureFromLod);

        // Wrap in a base texture for easy binding.
        auto lodTexture          = BaseTexture::New(std::nullopt);
        lodTexture->isCube       = true;
        lodTexture->_texture     = glTextureFromLod;
        lodTextures[mipmapIndex] = lodTexture;

        switch (i) {
          case 0:
            texture->_lodTextureLow = lodTexture;
            break;
          case 1:
            texture->_lodTextureMid = lodTexture;
            break;
          case 2:
            texture->_lodTextureHigh = lodTexture;
            break;
        }
      }
    }
  }

  std::vector<std::function<void()>> promises;
  // All mipmaps up to provided number of images
  for (size_t i = 0; i < imageData.size(); ++i) {
    // All faces
    for (unsigned int face = 0; face < 6; ++face) {
      // Enqueue promise to upload to the texture.
      const auto promise = [=]() -> void {
        // Constructs an image element from image data
        const auto& bytes = imageData[i][face];
        auto image        = FileTools::ArrayBufferToImage(bytes);
        std::string url;

        _OnImageReadySync(image, engine, expandTexture, rgbdPostProcess, url, face,
                          static_cast<int>(i), generateNonLODTextures, lodTextures, cubeRtt,
                          texture);
      };

      promises.emplace_back(promise);
    }
  }

  // Fill remaining mipmaps with black textures.
  if (imageData.size() < mipmapsCount) {
    ArrayBufferView data;
    auto size       = std::pow(2, mipmapsCount - 1 - imageData.size());
    auto dataLength = static_cast<size_t>(size * size * 4);
    switch (texture->type) {
      case Constants::TEXTURETYPE_UNSIGNED_INT: {
        data = Uint8Array(dataLength);
        break;
      }
      case Constants::TEXTURETYPE_HALF_FLOAT: {
        data = Uint16Array(dataLength);
        break;
      }
      case Constants::TEXTURETYPE_FLOAT: {
        data = Float32Array(dataLength);
        break;
      }
    }
    for (size_t i = imageData.size(); i < mipmapsCount; ++i) {
      for (unsigned int face = 0; face < 6; face++) {
        engine->_uploadArrayBufferViewToTexture(texture, data.uint8Array(), face,
                                                static_cast<int>(i));
      }
    }
  }

  // Once all done, finishes the cleanup and return
  for (const auto& promise : promises) {
    promise();
  }

  // Release temp RTT.
  if (cubeRtt) {
    engine->_releaseFramebufferObjects(cubeRtt);
    engine->_releaseTexture(texture);
    cubeRtt->_swapAndDie(texture);
  }
  // Release temp Post Process.
  if (rgbdPostProcess) {
    rgbdPostProcess->dispose();
  }
  // Flag internal texture as ready in case they are in use.
  if (generateNonLODTextures) {
    if (texture->_lodTextureHigh && texture->_lodTextureHigh->_texture) {
      texture->_lodTextureHigh->_texture->isReady = true;
    }
    if (texture->_lodTextureMid && texture->_lodTextureMid->_texture) {
      texture->_lodTextureMid->_texture->isReady = true;
    }
    if (texture->_lodTextureLow && texture->_lodTextureLow->_texture) {
      texture->_lodTextureLow->_texture->isReady = true;
    }
  }
}

void EnvironmentTextureTools::UploadEnvSpherical(const InternalTexturePtr& texture,
                                                 const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    BABYLON_LOGF_WARN("EnvironmentTextureTools",
                      "Unsupported babylon environment map version \"%u\"", info.version)
  }

  auto irradianceInfo = info.irradiance;
  if (!irradianceInfo) {
    return;
  }

  auto sp = std::make_shared<SphericalPolynomial>();
  Vector3::FromArrayToRef(irradianceInfo->x, 0, sp->x);
  Vector3::FromArrayToRef(irradianceInfo->y, 0, sp->y);
  Vector3::FromArrayToRef(irradianceInfo->z, 0, sp->z);
  Vector3::FromArrayToRef(irradianceInfo->xx, 0, sp->xx);
  Vector3::FromArrayToRef(irradianceInfo->yy, 0, sp->yy);
  Vector3::FromArrayToRef(irradianceInfo->zz, 0, sp->zz);
  Vector3::FromArrayToRef(irradianceInfo->yz, 0, sp->yz);
  Vector3::FromArrayToRef(irradianceInfo->zx, 0, sp->zx);
  Vector3::FromArrayToRef(irradianceInfo->xy, 0, sp->xy);
  texture->_sphericalPolynomial = sp;
}

void EnvironmentTextureTools::_UpdateRGBDSync(const InternalTexturePtr& internalTexture,
                                              const std::vector<std::vector<ArrayBuffer>>& data,
                                              const SphericalPolynomialPtr& sphericalPolynomial,
                                              float lodScale, float lodOffset)
{
  internalTexture->_source               = InternalTextureSource::CubeRawRGBD;
  internalTexture->_bufferViewArrayArray = data;
  internalTexture->_lodGenerationScale   = lodScale;
  internalTexture->_lodGenerationOffset  = lodOffset;
  internalTexture->_sphericalPolynomial  = sphericalPolynomial;

  EnvironmentTextureTools::UploadLevelsSync(internalTexture, data);
  internalTexture->isReady = true;
}

} // end of namespace BABYLON
