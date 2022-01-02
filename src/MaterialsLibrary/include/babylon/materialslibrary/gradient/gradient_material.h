#ifndef BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/gradient/gradient_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class GradientMaterial;
using GradientMaterialPtr = std::shared_ptr<GradientMaterial>;

class BABYLON_SHARED_EXPORT GradientMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static GradientMaterialPtr New(Ts&&... args)
  {
    auto material
      = std::shared_ptr<GradientMaterial>(new GradientMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~GradientMaterial() override; // = default

  /**
   * Returns wehter or not the grid requires alpha blending.
   */
  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  [[nodiscard]] std::string getClassName() const override;
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;
  [[nodiscard]] json serialize() const;

  /** Statics **/
  static GradientMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * Constructor
   * @param name The name given to the material in order to identify it
   * afterwards.
   * @param scene The scene the material is used in.
   */
  GradientMaterial(const std::string& name, Scene* scene);

  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);

public:
  Property<GradientMaterial, unsigned int> maxSimultaneousLights;
  // The gradient top color, red by default
  Color3 topColor;
  float topColorAlpha;
  Color3 bottomColor;
  float bottomColorAlpha;
  float offset;
  float scale;
  float smoothness;
  Property<GradientMaterial, bool> disableLighting;

private:
  unsigned int _maxSimultaneousLights;
  bool _disableLighting;

}; // end of class GradientMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H
