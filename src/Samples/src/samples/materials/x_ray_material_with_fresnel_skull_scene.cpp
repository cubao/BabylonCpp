#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of fresnel parameters to simulate an x-ray effect.
 * @see https://www.babylonjs-playground.com/#GK7FK#0
 * @see https://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
 */
class XRayMaterialWithFresnelSkullScene : public IRenderableScene {

public:
  XRayMaterialWithFresnelSkullScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _camera{nullptr}, _xrayMat{nullptr}
  {
  }

  ~XRayMaterialWithFresnelSkullScene() override = default;

  const char* getName() override
  {
    return "X-ray Material with Fresnel Skull Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    _camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -15.f), scene);

    // This targets the camera to scene origin
    _camera->setTarget(Vector3(0.f, 2.f, 0.f));

    // This attaches the camera to the canvas
    _camera->attachControl(canvas, true);

    // xray material
    _xrayMat                = StandardMaterial::New("xray", scene);
    _xrayMat->emissiveColor = Color3(1.f, 1.f, 1.f);
    _xrayMat->alpha         = 0.1f;

    // Set emissive fresnel parameters
    auto& emissiveFresnelParameters      = *_xrayMat->emissiveFresnelParameters();
    emissiveFresnelParameters.isEnabled  = true;
    emissiveFresnelParameters.leftColor  = Color3(0.5f, 0.6f, 1.f);
    emissiveFresnelParameters.rightColor = Color3(0.f, 0.f, 0.f);
    emissiveFresnelParameters.power      = 2.f;
    emissiveFresnelParameters.bias       = 0.1f;

    // Set opacity fresnel parameters
    auto& opacityFresnelParameters      = *_xrayMat->opacityFresnelParameters();
    opacityFresnelParameters.isEnabled  = true;
    opacityFresnelParameters.leftColor  = Color3(1.f, 1.f, 1.f);
    opacityFresnelParameters.rightColor = Color3(0.2f, 0.2f, 0.2f);
    opacityFresnelParameters.power      = 2.f;
    opacityFresnelParameters.bias       = 0.5f;

    // The first parameter can be used to specify which mesh to import. Here we
    // import all meshes
    SceneLoader::ImportMesh({}, "scenes/", "skull.babylon", scene,
                            [this](const std::vector<AbstractMeshPtr>& newMeshes,
                                   const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                   const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                   const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/,
                                   const std::vector<TransformNodePtr>& /*transformNodes*/,
                                   const std::vector<GeometryPtr>& /*geometries*/,
                                   const std::vector<LightPtr>& /*lights*/) {
                              // Set the target of the camera to the first imported mesh
                              if (!newMeshes.empty()) {
                                auto mesh = newMeshes[0];

                                mesh->material = _xrayMat;
                                mesh->position = Vector3(0.f, 4.f, 0.f);
                                mesh->scaling  = Vector3(0.1f, 0.1f, 0.1f);

                                // reset mesh color
                                Float32Array colors;
                                auto count = mesh->getTotalVertices() * 4;
                                colors.reserve(count);
                                colors.resize(count);
                                for (size_t i = 0; i < count; i += 4) {
                                  colors[i]     = 1.f;
                                  colors[i + 1] = 1.f;
                                  colors[i + 2] = 1.f;
                                  colors[i + 3] = 1.f;
                                }
                                mesh->setVerticesData(VertexBuffer::ColorKind, colors);

                                // Our built-in 'ground' shape. Params: name, width, depth, subdivs,
                                // scene
                                auto ground         = Mesh::CreateBox("ground", 1, _scene.get());
                                ground->scaling().x = 10.f;
                                ground->scaling().z = 10.f;
                                ground->material    = _xrayMat;
                              }
                            });
  }

private:
  FreeCameraPtr _camera;
  StandardMaterialPtr _xrayMat;

}; // end of class XRayMaterialWithFresnelSkullScene

BABYLON_REGISTER_SAMPLE("Materials", XRayMaterialWithFresnelSkullScene)

} // end of namespace Samples
} // end of namespace BABYLON
