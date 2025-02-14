#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Multisample Render Targets Scene. Example demonstrating how to use multisample render
 * targets.
 * @see https://www.babylonjs-playground.com/#12MKMN
 */
class MultiSampleRenderTargetsScene : public IRenderableScene {

public:
  MultiSampleRenderTargetsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MultiSampleRenderTargetsScene() override = default;

  const char* getName() override
  {
    return "Multisample Render Targets Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 10.f, Vector3::Zero(), scene);

    camera->setPosition(Vector3(0.f, 5.f, -10.f));
    camera->attachControl(canvas, true);

    camera->upperBetaLimit   = Math::PI_2;
    camera->lowerRadiusLimit = 4.f;

    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.7f;

    auto knot = Mesh::CreateTorusKnot("knot", 1, 0.4f, 128, 64, 2, 3, scene);

    // Mirror
    auto mirror                       = Mesh::CreateBox("Mirror", 1.0, scene);
    mirror->scaling                   = Vector3(100.f, 0.01f, 100.f);
    auto mirrorMaterial               = StandardMaterial::New("mirror", scene);
    auto reflectionTexture            = MirrorTexture::New("mirror", 512.f, scene, true);
    reflectionTexture->mirrorPlane    = Plane(0.f, -1.f, 0.f, -2.f);
    reflectionTexture->renderList     = {knot.get()};
    reflectionTexture->level          = 1.f;
    reflectionTexture->samples        = 8;
    mirrorMaterial->reflectionTexture = reflectionTexture;
    mirror->material                  = mirrorMaterial;
    mirror->position                  = Vector3(0.f, -2.f, 0.f);

    // Main material
    auto mainMaterial            = StandardMaterial::New("main", scene);
    knot->material               = mainMaterial;
    mainMaterial->diffuseTexture = Texture::New("textures/amiga.jpg", scene);

    // Fog
    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogColor = scene->clearColor;
    scene->fogStart = 20.f;
    scene->fogEnd   = 50.f;
  }

}; // end of class MultiSampleRenderTargetsScene

BABYLON_REGISTER_SAMPLE("Textures", MultiSampleRenderTargetsScene)

} // end of namespace Samples
} // end of namespace BABYLON
