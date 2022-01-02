#ifndef BABYLON_GAMEPADS_GAMEPAD_POSE_H
#define BABYLON_GAMEPADS_GAMEPAD_POSE_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/vr/device_pose.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT GamepadPose : public DevicePose {
  bool hasOrientation;
  bool hasPosition;
}; // end of struct GamepadPose

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_GAMEPAD_POSE_H
