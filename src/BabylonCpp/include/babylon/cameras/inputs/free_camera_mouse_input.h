#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

/**
 * @brief Manage the mouse inputs to control the movement of a free camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FreeCameraMouseInput : public ICameraInput<FreeCamera> {

public:
  /**
   * @brief Manage the mouse inputs to control the movement of a free camera.
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   * @param touchEnabled Defines if touch is enabled or not
   */
  FreeCameraMouseInput(bool touchEnabled = true);
  ~FreeCameraMouseInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input from.
   * @param noPreventDefault Defines whether event caught by the controls should call
   * preventdefault() (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param ignored defines an ignored parameter kept for backward compatibility. If you want to
   * define the source input element, you can set engine.inputElement before calling
   * camera.attachControl
   */
  void detachControl(ICanvas* ignored = nullptr) override;

  /**
   * @brief Update the current camera state depending on the inputs that have been used this frame.
   * This is a dynamically created lambda to avoid the performance penalty of looping for inputs in
   * the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  [[nodiscard]] std::string getSimpleName() const override;

protected:
  /**
   * @brief Called on JS contextmenu event.
   * Override this method to provide functionality.
   */
  virtual void onContextMenu(PointerEvent& evt);

public:
  /**
   * Defines the buttons associated with the input to handle camera move.
   */
  std::array<MouseButtonType, 3> buttons;

  /**
   * Defines the pointer angular sensibility  along the X and Y axis or how fast
   * is the camera rotating.
   */
  float angularSensibility;

  /**
   * Observable for when a pointer move event occurs containing the move offset
   */
  Observable<PointerEvent> onPointerMovedObservable;

  /**
   * Hidden
   * If the camera should be rotated automatically based on pointer movement
   */
  bool _allowCameraRotation;

  /**
   * Define if touch is enabled in the mouse input
   */
  bool touchEnabled;

private:
  ICanvas* _canvas;
  Engine* _engine;
  std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  std::function<void(MouseEvent& e)> _onMouseMove;
  Observer<PointerInfo>::Ptr _observer;
  std::optional<PositionCoord> previousPosition;
  bool _noPreventDefault;

}; // end of class FreeCameraMouseInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H
