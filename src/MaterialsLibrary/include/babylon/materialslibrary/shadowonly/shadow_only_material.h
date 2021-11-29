#ifndef ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H
#define ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/shadowonly/shadow_only_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

FWD_CLASS_SPTR(IShadowLight)

namespace MaterialsLibrary {

FWD_CLASS_SPTR(ShadowOnlyMaterial)

class BABYLON_SHARED_EXPORT ShadowOnlyMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static ShadowOnlyMaterialPtr New(Ts&&... args)
  {
    auto material
      = std::shared_ptr<ShadowOnlyMaterial>(new ShadowOnlyMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~ShadowOnlyMaterial() override; // = default

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::string getClassName() const override;
  MaterialPtr clone(const std::string& name, bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static ShadowOnlyMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Constructor
   * @param name The name given to the material in order to identify it afterwards.
   * @param scene The scene the material is used in.
   */
  ShadowOnlyMaterial(const std::string& name, Scene* scene);

  IShadowLightPtr& get_activeLight();
  void set_activeLight(const IShadowLightPtr& light);

private:
  IShadowLightPtr _getFirstShadowLightForMesh(AbstractMesh* mesh);

public:
  Color3 shadowColor;
  Property<ShadowOnlyMaterial, IShadowLightPtr> activeLight;

private:
  IShadowLightPtr _activeLight;
  bool _needAlphaBlending;

}; // end of class ShadowOnlyMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H
