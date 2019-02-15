#ifndef BABYLON_SAMPLES_ANIMATIONS_EASING_EASING_FUNCTIONS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_EASING_EASING_FUNCTIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Easing Functions Scen. Example demonstarting how to create animations
 * with different easing functions.
 * @see https://www.babylonjs-playground.com/#8ZNVGR#0
 * @see https://doc.babylonjs.com/babylon101/animations#easing-functions
 */
struct EasingFunctionsScene : public IRenderableScene {

  EasingFunctionsScene(ICanvas* iCanvas);
  ~EasingFunctionsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct EasingFunctionsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_EASING_EASING_FUNCTIONS_SCENE_H
