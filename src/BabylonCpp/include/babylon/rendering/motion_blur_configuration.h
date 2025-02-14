#ifndef BABYLON_RENDERING_MOTION_BLUR_CONFIGURATION_H
#define BABYLON_RENDERING_MOTION_BLUR_CONFIGURATION_H

#include <vector>

#include <babylon/engines/constants.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>

namespace BABYLON {

/**
 * @brief Contains all parameters needed for the prepass to perform motion blur.
 */
struct BABYLON_SHARED_EXPORT MotionBlurConfiguration : public PrePassEffectConfiguration {
  /**
   * Is motion blur enabled
   */
  bool enabled = false;

  /**
   * @brief Returns the name of the configuration.
   */
  std::string name() const override
  {
    return "motionBlur";
  }

  /**
   * @brief Textures that should be present in the MRT for this effect to work
   */
  std::vector<uint32_t> texturesRequired() const override
  {
    return {Constants::PREPASS_VELOCITY_TEXTURE_TYPE};
  };

}; // end of struct MotionBlurConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_MOTION_BLUR_CONFIGURATION_H
