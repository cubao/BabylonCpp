#include <babylon/materials/effect_wrapper.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/materials/draw_wrapper.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_wrapper_creation_options.h>
#include <babylon/materials/ieffect_creation_options.h>

namespace BABYLON {

EffectWrapper::EffectWrapper(const EffectWrapperCreationOptions& creationOptions)
    : effect{this, &EffectWrapper::get_effect, &EffectWrapper::set_effect}
    , _onContextRestoredObserver{nullptr}
{
  std::unordered_map<std::string, std::string> effectCreationOptions{};
  auto uniformNames = creationOptions.uniformNames;
  if (!creationOptions.vertexShader.empty()) {
    effectCreationOptions["fragmentSource"] = creationOptions.fragmentShader;
    effectCreationOptions["vertexSource"]   = creationOptions.vertexShader;
    effectCreationOptions["spectorName"]
      = !creationOptions.name.empty() ? creationOptions.name : "effectWrapper";
  }
  else {
    // Default scale to use in post process vertex shader.
    uniformNames.emplace_back("scale");

    effectCreationOptions["fragmentSource"] = creationOptions.fragmentShader;
    effectCreationOptions["vertexSource"]   = "postprocess";
    effectCreationOptions["spectorName"]
      = !creationOptions.name.empty() ? creationOptions.name : "effectWrapper";

    // Sets the default scale to identity for the post process vertex shader.
    onApplyObservable.add(
      [this](void*, EventState& /*es*/) -> void { effect()->setFloat2("scale", 1.f, 1.f); });
  }

  _drawWrapper = std::make_shared<DrawWrapper>(creationOptions.engine);

  const std::vector<std::string> fallbackAttributeNames = {"position"};

  IEffectCreationOptions options;
  options.attributes    = !creationOptions.attributeNames.empty() ? creationOptions.attributeNames :
                                                                    fallbackAttributeNames;
  options.uniformsNames = uniformNames;
  options.samplers      = creationOptions.samplerNames;

  effect = Effect ::New(effectCreationOptions, options, creationOptions.engine);

  _onContextRestoredObserver = creationOptions.engine->onContextRestoredObservable.add(
    [this](ThinEngine* /*engine*/, EventState& /*es*/) -> void {
      effect()->_pipelineContext
        = nullptr; // because _prepareEffect will try to dispose this pipeline before recreating it
                   // and that would lead to webgl errors
      effect()->_wasPreviouslyReady = false;
      effect()->_prepareEffect();
    });
}

EffectWrapper::~EffectWrapper() = default;

EffectPtr& EffectWrapper::get_effect()
{
  return _drawWrapper->effect;
}

void EffectWrapper::set_effect(const EffectPtr& iEffect)
{
  _drawWrapper->effect = iEffect;
}

void EffectWrapper::dispose()
{
  if (_onContextRestoredObserver) {
    effect()->getEngine()->onContextRestoredObservable.remove(_onContextRestoredObserver);
    _onContextRestoredObserver = nullptr;
  }
  effect()->dispose();
}

} // end of namespace BABYLON
