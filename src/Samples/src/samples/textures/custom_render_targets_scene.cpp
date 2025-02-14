#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ishader_material_options.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Custom render targets scene. Example demonstrating how to use render target textures to
 * generate procedural data.
 * @see https://www.babylonjs-playground.com/#CJWDJR#0
 */
class CustomRenderTargetsScene : public IRenderableScene {

public:
  static constexpr const char* customDepthVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "attribute vec3 position;\n"
      "uniform mat4 worldViewProjection;\n"
      "void main(void) {\n"
      "gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "}";

  static constexpr const char* customDepthPixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "void main(void) {\n"
      "float depth =  1.0 - \n"
      "              (2.0 / (100.0 + 1.0 - gl_FragCoord.z * (100.0 - 1.0)));\n"
      "gl_FragColor = vec4(depth, depth, depth, 1.0);\n"
      "}\n"
      "";

public:
  CustomRenderTargetsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _camera{nullptr}, _renderTarget{nullptr}, _depthMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customDepthVertexShader"] = customDepthVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customDepthPixelShader"] = customDepthPixelShader;
  }

  ~CustomRenderTargetsScene() override = default;

  const char* getName() override
  {
    return "Custom Render Targets Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto engine            = scene->getEngine();
    _camera                = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    auto material          = StandardMaterial::New("kosh", scene);
    material->diffuseColor = Color3::Purple();
    auto light             = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);

    _camera->setPosition(Vector3(-15.f, 10.f, -20.f));
    _camera->minZ = 1.f;
    _camera->maxZ = 120.f;

    _camera->attachControl(canvas, true);

    // Skybox
    auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
    auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling = false;
    skyboxMaterial->reflectionTexture
      = CubeTexture::New("/textures/skybox/TropicalSunnyDay", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->disableLighting                      = true;
    skybox->material                                     = skyboxMaterial;

    // Depth material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position"};
    shaderMaterialOptions.uniforms   = {"worldViewProjection"};
    _depthMaterial
      = ShaderMaterial::New("customDepth", scene, "customDepth", shaderMaterialOptions);
    _depthMaterial->backFaceCulling = false;

    // Plane
    auto plane           = Mesh::CreatePlane("map", 10, scene);
    plane->billboardMode = AbstractMesh::BILLBOARDMODE_ALL;
    plane->scaling().y   = 1.f / engine->getAspectRatio(*scene->activeCamera());

    // Render target
    _renderTarget = RenderTargetTexture::New("depth", 1024.f, scene, true);
    _renderTarget->renderList().emplace_back(skybox.get());
    scene->customRenderTargets.emplace_back(_renderTarget);

    _renderTarget->onBeforeRender = [this](int* /*faceIndex*/, EventState& /*es*/) {
      for (auto& renderItem : _renderTarget->renderList()) {
        renderItem->_savedMaterial = renderItem->material;
        renderItem->material       = _depthMaterial;
      }
    };

    _renderTarget->onAfterRender = [this](int* /*faceIndex*/, EventState& /*es*/) {
      // Restoring previous material
      for (auto& renderItem : _renderTarget->renderList()) {
        renderItem->material = renderItem->_savedMaterial;
      }
    };

    // Spheres
    size_t spheresCount = 20;
    float alpha         = 0.f;
    for (size_t index = 0; index < spheresCount; ++index) {
      auto indexStr        = std::to_string(index);
      auto sphere          = Mesh::CreateSphere("Sphere" + indexStr, 32, 3, scene);
      sphere->position().x = 10.f * std::cos(alpha);
      sphere->position().z = 10.f * std::sin(alpha);
      sphere->material     = material;

      alpha += Math::PI2 / spheresCount;

      _renderTarget->renderList().emplace_back(sphere.get());
    }

    // Plane material
    auto mat             = StandardMaterial::New("planeMat", scene);
    mat->emissiveTexture = _renderTarget;
    mat->disableLighting = true;

    plane->material = mat;

    // Animations
    scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
      _camera->alpha += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  ArcRotateCameraPtr _camera;
  RenderTargetTexturePtr _renderTarget;
  ShaderMaterialPtr _depthMaterial;

}; // end of class CustomRenderTargetsScene

BABYLON_REGISTER_SAMPLE("Textures", CustomRenderTargetsScene)

} // end of namespace Samples
} // end of namespace BABYLON
