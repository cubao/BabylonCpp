#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Box Instances Scene. Example demonstrating how to create instances.
 */
struct BoxInstancesScene : public IRenderableScene {

  BoxInstancesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~BoxInstancesScene() override = default;

  const char* getName() override
  {
    return "Box Instances Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 8.f, 50.f,
                                       Vector3::Zero(), scene);

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light       = HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);
    light->diffuse   = Color3::FromInts(246, 135, 18); // "0xf68712"
    light->specular  = Color3::FromInts(241, 71, 29);  // "0xf1471d"
    light->intensity = 1.f;

    // Create a box
    // (name of the box, size, scene)
    auto box = Mesh::CreateBox("box", 6.f, scene);

    // Create box instances
    auto boxInstance1 = box->createInstance("boxInstance1");
    auto boxInstance2 = box->createInstance("boxInstance2");

    // Moving elements
    box->position          = Vector3(-10.f, 0.f, 0.f);
    boxInstance1->position = Vector3(0.f, 0.f, 0.f);
    boxInstance2->position = Vector3(10.f, 0.f, 0.f);
  }

}; // end of struct BoxInstancesScene

BABYLON_REGISTER_SAMPLE("Optimizations", BoxInstancesScene)

} // end of namespace Samples
} // end of namespace BABYLON
