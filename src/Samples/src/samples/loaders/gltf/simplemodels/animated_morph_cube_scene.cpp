#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/light.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Animated Morph Cube Scene (glTF). The cube contains two morph targets in it:
 * * thin : moves the vertices so that it becomes super-thin
 * * angle : moves the vertices so that it forms a ramp
 * The animation loops, morphing between these two states.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedMorphCube
 */
struct AnimatedMorphCubeScene : public IRenderableScene {

  AnimatedMorphCubeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~AnimatedMorphCubeScene() override = default;

  const char* getName() override
  {
    return "Animated Morph Cube Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/AnimatedMorphCube/glTF/", "AnimatedMorphCube.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/,
              const std::vector<TransformNodePtr>& /*transformNodes*/,
              const std::vector<GeometryPtr>& /*geometries*/,
              const std::vector<LightPtr>& /*lights*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(0.f, 0.0f, 0.f));
          camera->alpha  = Math::PI / 12.f;
          camera->beta   = Math::PI / 3.f;
          camera->radius = Math::PI2;
        }
      });
  }

}; // end of struct AnimatedMorphCubeScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", AnimatedMorphCubeScene)

} // end of namespace Samples
} // end of namespace BABYLON
