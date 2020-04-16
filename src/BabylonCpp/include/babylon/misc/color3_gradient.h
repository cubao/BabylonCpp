#ifndef BABYLON_MISC_COLOR3_GRADIENT_H
#define BABYLON_MISC_COLOR3_GRADIENT_H

#include <babylon/babylon_api.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store color3 gradient.
 */
struct BABYLON_SHARED_EXPORT Color3Gradient : public IValueGradient {

  /**
   * @brief Creates a new color3 gradient.
   * @param gradient gets or sets the gradient value (between 0 and 1)
   * @param color gets or sets associated color
   */
  Color3Gradient(float gradient, const Color3& color);
  ~Color3Gradient(); // = default

  /**
   * Gets or sets the associated color
   */
  Color3 color;

}; // end of struct Color3Gradient

bool operator==(const Color3Gradient& lhs, const Color3Gradient& rhs);
bool operator!=(const Color3Gradient& lhs, const Color3Gradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_COLOR3_GRADIENT_H
