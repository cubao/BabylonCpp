#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/perlinnoise/perlin_noise_procedural_texture.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PerlinNoiseProceduralTextureScene Scene. Example demonstrating how to use perlin noise
 * procedural texture.
 */
struct PerlinNoiseProceduralTextureScene : public IRenderableScene {

  PerlinNoiseProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PerlinNoiseProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Perlin Noise Procedural Texture Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2, 25.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto perlinnoiseMaterial = StandardMaterial::New("PerlinNoiseMaterial", scene);
    auto perlinnoiseTexture  = ProceduralTexturesLibrary::PerlinNoiseProceduralTexture::New(
      "PerlinNoiseMaterialtext", 512, scene);
    perlinnoiseMaterial->diffuseTexture = perlinnoiseTexture;

    auto plane      = Mesh::CreatePlane("PerlinNoisePlane", 20, scene);
    plane->material = perlinnoiseMaterial;
    plane->rotate(Vector3(1.f, 1.f, 0.5f), Math::PI / 3.f, Space::LOCAL);
  }

}; // end of struct PerlinNoiseProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", PerlinNoiseProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
