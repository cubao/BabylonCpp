#ifndef BABYLON_RENDERING_PRE_PASS_RENDERER_H
#define BABYLON_RENDERING_PRE_PASS_RENDERER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

class Camera;
class Effect;
class Engine;
class Scene;
class SubMesh;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(GeometryBufferRenderer)
FWD_CLASS_SPTR(ImageProcessingPostProcess)
FWD_CLASS_SPTR(Material)
FWD_CLASS_SPTR(MultiRenderTarget)
FWD_CLASS_SPTR(PostProcess)
FWD_STRUCT_SPTR(PrePassEffectConfiguration)
FWD_CLASS_SPTR(PrePassRenderTarget)
FWD_CLASS_SPTR(RenderTargetTexture)
FWD_CLASS_SPTR(SubSurfaceConfiguration)

struct TextureFormatMapping {
  unsigned int type   = 0;
  unsigned int format = 0;
  std::string name    = "";
}; // end of struct TextureFormatMapping

/**
 * @brief Renders a pre pass of the scene.
 * This means every mesh in the scene will be rendered to a render target texture
 * And then this texture will be composited to the rendering canvas with post processes
 * It is necessary for effects like subsurface scattering or deferred shading
 */
class BABYLON_SHARED_EXPORT PrePassRenderer : public std::enable_shared_from_this<PrePassRenderer> {

public:
  /**
   * @brief Instanciates a prepass renderer.
   * @param scene The scene
   */
  PrePassRenderer(Scene* scene);
  ~PrePassRenderer() = default;

  /**
   * @brief Creates a new PrePassRenderTarget.
   * This should be the only way to instanciate a `PrePassRenderTarget`
   * @param name Name of the `PrePassRenderTarget`
   * @param renderTargetTexture RenderTarget the `PrePassRenderTarget` will be attached to.
   * Can be `null` if the created `PrePassRenderTarget` is attached to the scene (default
   * framebuffer).
   * @hidden
   */
  PrePassRenderTargetPtr _createRenderTarget(const std::string& name,
                                             const RenderTargetTexturePtr& renderTargetTexture);

  /**
   * @return the prepass render target for the rendering pass.
   * If we are currently rendering a render target, it returns the PrePassRenderTarget
   * associated with that render target. Otherwise, it returns the scene default PrePassRenderTarget
   */
  PrePassRenderTargetPtr& getRenderTarget();

  /**
   * @hidden
   * @brief Managed by the scene component
   * @param prePassRenderTarget
   */
  void _setRenderTarget(const PrePassRenderTargetPtr& prePassRenderTarget);

  /**
   * @brief Sets the proper output textures to draw in the engine.
   * @param effect The effect that is drawn. It can be or not be compatible with drawing to several
   * output textures.
   */
  void bindAttachmentsForEffect(Effect& effect, SubMesh* subMesh);

  /**
   * @brief Restores attachments for single texture draw.
   */
  void restoreAttachments();

  /**
   * @hidden
   */
  void _beforeDraw(Camera* camera = nullptr, int faceIndex = -1, int layer = -1);

  /**
   * @brief Hidden
   */
  void _afterDraw(int faceIndex = -1, int layer = -1);

  /**
   * @brief Clears the scene render target (in the sense of settings pixels to the scene clear color
   * value).
   */
  void _clear();

  /**
   * @brief Adds an effect configuration to the prepass render target.
   * If an effect has already been added, it won't add it twice and will return the configuration
   * already present.
   * @param cfg the effect configuration
   * @return the effect configuration now used by the prepass
   */
  PrePassEffectConfigurationPtr addEffectConfiguration(const PrePassEffectConfigurationPtr& cfg);

  /**
   * @brief Returns the index of a texture in the multi render target texture array.
   * @param type Texture type
   * @return The index
   */
  int getIndex(unsigned int type);

  /**
   *  @brief Hidden
   */
  void _unlinkInternalTexture(const PrePassRenderTargetPtr& prePassRenderTarget);

  /**
   * @brief Marks the prepass renderer as dirty, triggering a check if the prepass is necessary for
   * the next rendering.
   */
  void markAsDirty();

  /**
   * @brief Disposes the prepass renderer.
   */
  void dispose();

protected:
  /**
   * @brief Gets How many samples are used for MSAA of the scene render target.
   */
  unsigned int get_samples() const;

  /**
   * @brief Sets How many samples are used for MSAA of the scene render target.
   */
  void set_samples(unsigned int n);

  /**
   * @brief Returns true if the currently rendered prePassRenderTarget is the one
   * associated with the scene.
   */
  bool get_currentRTisSceneRT() const;

  /**
   * @brief Indicates if the prepass is enabled.
   */
  bool get_enabled() const;

  /**
   * @brief Indicates if rendering a prepass is supported.
   */
  bool get_isSupported() const;

private:
  void _refreshGeometryBufferRendererLink();
  void _reinitializeAttachments();
  void _resetLayout();
  void _updateGeometryBufferLayout();
  void _prepareFrame(const PrePassRenderTargetPtr& prePassRenderTarget, int faceIndex = -1,
                     int layer = -1);
  void _renderPostProcesses(const PrePassRenderTargetPtr& prePassRenderTarget, int faceIndex = -1);
  void _bindFrameBuffer(const PrePassRenderTargetPtr& prePassRenderTarget);
  void _setEnabled(bool enabled);
  void _setRenderTargetEnabled(const PrePassRenderTargetPtr& prePassRenderTarget, bool enabled);
  void _enable();
  void _disable();
  std::vector<PostProcessPtr>
  _getPostProcessesSource(const PrePassRenderTargetPtr& prePassRenderTarget,
                          Camera* camera = nullptr);
  void _setupOutputForThisPass(const PrePassRenderTargetPtr& prePassRenderTarget,
                               Camera* camera = nullptr);
  void _linkInternalTexture(const PrePassRenderTargetPtr& prePassRenderTarget,
                            const PostProcessPtr& postProcess);
  bool _needsImageProcessing() const;
  bool _hasImageProcessing(const std::vector<PostProcessPtr>& postProcesses) const;

  /**
   * @brief Internal, gets the first post proces.
   * @returns the first post process to be run on this camera.
   */
  PostProcessPtr _getFirstPostProcess(const std::vector<PostProcessPtr>& postProcesses);

  /**
   * @brief Enables a texture on the MultiRenderTarget for prepass.
   */
  void _enableTextures(const std::vector<unsigned int>& types);

  void _update();
  void _markAllMaterialsAsPrePassDirty();

public:
  /**
   * To save performance, we can excluded skinned meshes from the prepass
   */
  std::vector<AbstractMeshPtr> excludedSkinnedMesh;

  /**
   * Force material to be excluded from the prepass
   * Can be useful when `useGeometryBufferFallback` is set to `true`
   * and you don't want a material to show in the effect.
   */
  std::vector<MaterialPtr> excludedMaterials;

  /**
   * Number of textures in the multi render target texture where the scene is directly rendered
   */
  size_t mrtCount;

  /**
   * How many samples are used for MSAA of the scene render target
   */
  Property<PrePassRenderer, unsigned int> samples;

  /**
   * The render target where the scene is directly rendered
   */
  PrePassRenderTargetPtr defaultRT;

  /**
   * Returns true if the currently rendered prePassRenderTarget is the one associated with the
   * scene.
   */
  ReadOnlyProperty<PrePassRenderer, bool> currentRTisSceneRT;

  /**
   * Prevents the PrePassRenderer from using the GeometryBufferRenderer as a fallback
   */
  bool doNotUseGeometryRendererFallback;

  /**
   * All the render targets generated by prepass
   */
  std::vector<PrePassRenderTargetPtr> renderTargets;

  /**
   * Indicates if the prepass is enabled
   */
  ReadOnlyProperty<PrePassRenderer, bool> enabled;

  /**
   * Set to true to disable gamma transform in PrePass.
   * Can be useful in case you already proceed to gamma transform on a material level
   * and your post processes don't need to be in linear color space.
   */
  bool disableGammaTransform;

  /**
   * Indicates if rendering a prepass is supported
   */
  ReadOnlyProperty<PrePassRenderer, bool> isSupported;

private:
  Scene* _scene;
  Engine* _engine;
  static std::vector<TextureFormatMapping> _textureFormats;
  Uint32Array _multiRenderAttachments;
  Uint32Array _defaultAttachments;
  Uint32Array _clearAttachments;
  bool _isDirty;

  /**
   * Configuration for prepass effects
   */
  std::vector<PrePassEffectConfigurationPtr> _effectConfigurations;

  Uint32Array _mrtFormats;
  Uint32Array _mrtLayout;
  std::vector<std::string> _mrtNames;
  Int32Array _textureIndices;

  GeometryBufferRendererPtr _geometryBuffer;
  PrePassRenderTargetPtr _currentTarget;

  const Color4 _clearColor;
  bool _enabled;
  bool _needsCompositionForThisPass = false;
  std::vector<PostProcessPtr> _postProcessesSourceForThisPass;

}; // end of class PrePassRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_RENDERER_H
