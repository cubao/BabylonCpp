#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Materials scene. Example demonstrating how to create and add materials to a mesh.
 * @see https://www.babylonjs-playground.com/#DXARSP#0
 * @see https://doc.babylonjs.com/babylon101/materials
 */
struct MaterialsScene : public IRenderableScene {

  MaterialsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MaterialsScene() override = default;

  const char* getName() override
  {
    return "Materials Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a light
    auto light       = PointLight::New("Omni", Vector3(-60.f, 60.f, 80.f), scene);
    light->intensity = 0.98f;

    // Create an Arc Rotate Camera - aimed negative z this time
    auto camera = ArcRotateCamera::New("Camera", Math::PI / 2, 1.f, 110.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Creation of 6 spheres
    auto sphere1 = Mesh::CreateSphere("Sphere1", 10u, 9.f, scene);
    auto sphere2 = Mesh::CreateSphere("Sphere2", 2u, 9.f, scene);
    auto sphere3 = Mesh::CreateSphere("Sphere3", 10u, 9.f, scene);
    auto sphere4 = Mesh::CreateSphere("Sphere4", 10u, 9.f, scene);
    auto sphere5 = Mesh::CreateSphere("Sphere5", 10u, 9.f, scene);
    auto sphere6 = Mesh::CreateSphere("Sphere6", 10u, 9.f, scene);

    // Position the spheres
    sphere1->position().x = 40.f;
    sphere2->position().x = 25.f;
    sphere3->position().x = 10.f;
    sphere4->position().x = -5.f;
    sphere5->position().x = -20.f;
    sphere6->position().x = -35.f;

    // Creation of a plane
    auto plane          = Mesh::CreatePlane("plane", 120, scene);
    plane->position().y = -5;
    plane->rotation().x = Math::PI_2;

    // Creation of a material with wireFrame
    auto materialSphere1       = StandardMaterial::New("texture1", scene);
    materialSphere1->wireframe = true;

    // Creation of a red material with alpha
    auto materialSphere2          = StandardMaterial::New("texture2", scene);
    materialSphere2->diffuseColor = Color3(1, 0, 0); // Red
    materialSphere2->alpha        = 0.3f;

    // Creation of a material with an image texture
    auto materialSphere3            = StandardMaterial::New("texture3", scene);
    materialSphere3->diffuseTexture = Texture::New("textures/misc.jpg", scene);

    // Creation of a material with translated texture
    auto materialSphere4            = StandardMaterial::New("texture4", scene);
    materialSphere4->diffuseTexture = Texture::New("textures/misc.jpg", scene);
    auto texture     = std::static_pointer_cast<Texture>(materialSphere4->diffuseTexture());
    texture->vOffset = 0.1f; // Vertical offset of 10%
    texture->uOffset = 0.4f; // Horizontal offset of 40%

    // Creation of a material with an alpha texture
    auto materialSphere5                        = StandardMaterial::New("texture5", scene);
    materialSphere5->diffuseTexture             = Texture::New("textures/tree.png", scene);
    materialSphere5->diffuseTexture()->hasAlpha = true; // Has an alpha

    // Creation of a material and show all the faces
    auto materialSphere6                        = StandardMaterial::New("texture6", scene);
    materialSphere6->diffuseTexture             = Texture::New("textures/tree.png", scene);
    materialSphere6->diffuseTexture()->hasAlpha = true;  // Have an alpha
    materialSphere6->backFaceCulling            = false; // Show all the faces of the element

    // Creation of a repeated textured material
    auto materialPlane             = StandardMaterial::New("texturePlane", scene);
    materialPlane->backFaceCulling = false; // Always show the front and the back of an element
    materialPlane->diffuseTexture  = Texture::New("textures/grass.jpg", scene);
    texture         = std::static_pointer_cast<Texture>(materialPlane->diffuseTexture());
    texture->uScale = 5.f;                  // Repeat 5 times on the Vertical Axes
    texture->vScale = 5.f;                  // Repeat 5 times on the Horizontal Axes
    materialPlane->backFaceCulling = false; // Always show the front and the back of an element

    // Apply the materials to meshes
    sphere1->material = materialSphere1;
    sphere2->material = materialSphere2;

    sphere3->material = materialSphere3;
    sphere4->material = materialSphere4;

    sphere5->material = materialSphere5;
    sphere6->material = materialSphere6;

    plane->material = materialPlane;
  }

}; // end of struct MaterialsScene

BABYLON_REGISTER_SAMPLE("Materials", MaterialsScene)

} // end of namespace Samples
} // end of namespace BABYLON
