#ifndef BABYLON_MATERIALS_EFFECT_RENDERER_H
#define BABYLON_MATERIALS_EFFECT_RENDERER_H

#include <memory>
#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

struct IEffectRendererOptions;
class ThinEngine;
class Viewport;
FWD_CLASS_SPTR(Effect)
FWD_STRUCT_SPTR(EffectWrapper)
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(RenderTargetTexture)
FWD_CLASS_SPTR(Texture)
FWD_CLASS_SPTR(VertexBuffer)
FWD_CLASS_SPTR(WebGLDataBuffer)

/**
 * @brief Helper class to render one or more effects.
 * You can access the previous rendering in your shader by declaring a sampler named textureSampler
 */
class BABYLON_SHARED_EXPORT EffectRenderer {

private:
  // Fullscreen quad buffers by default.
  static IEffectRendererOptions _DefaultOptions;

public:
  /**
   * @brief Creates an effect renderer
   * @param engine the engine to use for rendering
   * @param options defines the options of the effect renderer
   */
  EffectRenderer(ThinEngine* engine,
                 const IEffectRendererOptions& options = EffectRenderer::_DefaultOptions);
  ~EffectRenderer();

  /**
   * @brief Sets the current viewport in normalized coordinates 0-1.
   */
  void setViewport();

  /**
   * @brief Sets the current viewport in normalized coordinates 0-1.
   * @param viewport Defines the viewport to set (defaults to 0 0 1 1)
   */
  void setViewport(Viewport& viewport);

  /**
   * @brief Binds the embedded attributes buffer to the effect.
   * @param effect Defines the effect to bind the attributes for
   */
  void bindBuffers(const EffectPtr& effect);

  /**
   * @brief Sets the current effect wrapper to use during draw.
   * The effect needs to be ready before calling this api.
   * This also sets the default full screen position attribute.
   * @param effectWrapper Defines the effect to draw with
   */
  void applyEffectWrapper(const EffectWrapperPtr& effectWrapper);

  /**
   * @brief Restores engine states.
   */
  void restoreStates();

  /**
   * @brief Draws a full screen quad.
   */
  void draw();

  /**
   * @brief Renders one or more effects to a specified texture.
   * @param effectWrapper the effect to renderer
   * @param outputTexture texture to draw to, if null it will render to the screen.
   */
  void render(
    const EffectWrapperPtr& effectWrappers,
    const std::optional<std::variant<InternalTexturePtr, RenderTargetTexturePtr>>& outputTexture
    = std::nullopt);

  /**
   * @brief Disposes of the effect renderer.
   */
  void dispose();

private:
  bool
  isRenderTargetTexture(const std::variant<InternalTexturePtr, RenderTargetTexturePtr>& texture);

public:
  ThinEngine* engine;

private:
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  WebGLDataBufferPtr _indexBuffer;

  std::unique_ptr<Viewport> _fullscreenViewport;

}; // end of class EffectRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_RENDERER_H
