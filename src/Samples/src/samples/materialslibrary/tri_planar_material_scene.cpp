#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/triplanar/tri_planar_material.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the Tri-Planar material from the materials library.
 * Press the following keys:
 * - 1: to activate the Tri-Planar material (by default)
 * - 2: to active the default material
 * @see https://www.babylonjs-playground.com/#E6OZX#9
 * @see https://doc.babylonjs.com/extensions/Tri_Planar_Mapping
 */
class TriPlanarMaterialScene : public IRenderableScene {

public:
  TriPlanarMaterialScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _triPlanarMaterial{nullptr}
      , _defaultMaterial{nullptr}
      , _ground{nullptr}
  {
  }

  ~TriPlanarMaterialScene() override = default;

  const char* getName() override
  {
    return "Tri-Planar Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = FreeCamera::New("camera1", Vector3(5.f, 4.f, -47.f), scene);
    camera->setTarget(Vector3::Zero());
    camera->attachControl(canvas, true);

    // Light
    auto light       = HemisphericLight::New("light", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 1.f;

    // Create tri-planar material
    _triPlanarMaterial = MaterialsLibrary::TriPlanarMaterial::New("triplanar", scene);
    _triPlanarMaterial->diffuseTextureX = Texture::New("textures/rock.png", scene);
    _triPlanarMaterial->diffuseTextureY = Texture::New("textures/grass.png", scene);
    _triPlanarMaterial->diffuseTextureZ = Texture::New("textures/floor.png", scene);
    _triPlanarMaterial->normalTextureX  = Texture::New("textures/rockn.png", scene);
    _triPlanarMaterial->normalTextureY  = Texture::New("textures/grassn.png", scene);
    _triPlanarMaterial->normalTextureZ  = Texture::New("textures/rockn.png", scene);
    _triPlanarMaterial->specularPower   = 32.f;
    _triPlanarMaterial->tileSize        = 1.5f;

    // Create default material
    _defaultMaterial                 = StandardMaterial::New("ground", scene);
    auto groundTexture               = Texture::New("textures/ground.jpg", scene);
    groundTexture->uScale            = 10.f;
    groundTexture->vScale            = 10.f;
    _defaultMaterial->diffuseTexture = groundTexture;
    _defaultMaterial->specularColor  = Color3(0.f, 0.f, 0.f);

    // Ground
    _ground = Mesh::CreateGroundFromHeightMap("ground", "textures/heightMapTriPlanar.png", 100, 100,
                                              100, 0, 10, scene, false);
    _ground->material = _triPlanarMaterial;

    // Events
    canvas->addKeyEventListener(EventType::KEY_DOWN, [this](KeyboardEvent&& evt) {
      if (evt.keyCode == 49) {
        _ground->material = _triPlanarMaterial;
      }
      else if (evt.keyCode == 50) {
        _ground->material = _defaultMaterial;
      }
    });
  }

private:
  MaterialsLibrary::TriPlanarMaterialPtr _triPlanarMaterial;
  StandardMaterialPtr _defaultMaterial;
  GroundMeshPtr _ground;

}; // end of class TriPlanarMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", TriPlanarMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
