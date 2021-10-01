#ifndef BABYLON_MATERIALS_LIBRARY_CELL_CELL_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_CELL_CELL_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/cell/cell_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

FWD_CLASS_SPTR(IAnimatable)
FWD_CLASS_SPTR(Texture)

namespace MaterialsLibrary {

FWD_CLASS_SPTR(CellMaterial)

class BABYLON_SHARED_EXPORT CellMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static CellMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<CellMaterial>(new CellMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~CellMaterial() override; // = default

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;
  std::string getClassName() const override;
  MaterialPtr clone(const std::string& name, bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static CellMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  CellMaterial(const std::string& name, Scene* scene);

  TexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const TexturePtr& value);
  bool get_computeHighLevel() const;
  void set_computeHighLevel(bool value);
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<CellMaterial, TexturePtr> diffuseTexture;
  Color3 diffuseColor;
  Property<CellMaterial, bool> computeHighLevel;
  Property<CellMaterial, bool> disableLighting;
  Property<CellMaterial, unsigned int> maxSimultaneousLights;

private:
  TexturePtr _diffuseTexture;
  bool _computeHighLevel;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  Color3 _scaledDiffuse;

}; // end of class CellMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CELL_CELL_MATERIAL_H
