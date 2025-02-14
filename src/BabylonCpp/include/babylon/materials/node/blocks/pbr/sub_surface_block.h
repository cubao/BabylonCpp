#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_SUB_SURFACE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_SUB_SURFACE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(ReflectionBlock)
FWD_CLASS_SPTR(SubSurfaceBlock)

/**
 * @brief Block used to implement the sub surface module of the PBR material.
 */
class BABYLON_SHARED_EXPORT SubSurfaceBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static SubSurfaceBlockPtr New(Ts&&... args)
  {
    auto subSurfaceBlock
      = std::shared_ptr<SubSurfaceBlock>(new SubSurfaceBlock(std::forward<Ts>(args)...));
    SubSurfaceBlock::RegisterConnections(subSurfaceBlock);

    return subSurfaceBlock;
  }
  ~SubSurfaceBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const SubSurfaceBlockPtr& subSurfaceBlock);

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      SubMesh* subMesh = nullptr) override;

  /**
   * @brief Gets the main code of the block (fragment side).
   * @param state current state of the node material building
   * @param ssBlock instance of a SubSurfaceBlock or null if the code must be generated without an
   * active sub surface module
   * @param reflectionBlock instance of a ReflectionBlock null if the code must be generated without
   * an active reflection module
   * @param worldPosVarName name of the variable holding the world position
   * @returns the shader code
   */
  static std::string GetCode(NodeMaterialBuildState& state, const SubSurfaceBlockPtr& ssBlock,
                             const ReflectionBlockPtr& reflectionBlock,
                             const std::string& worldPosVarName);

protected:
  /**
   * @brief Creates a new SubSurfaceBlock.
   * @param name defines the block name
   */
  SubSurfaceBlock(const std::string& name);

  /**
   * @brief Gets the thickness component.
   */
  NodeMaterialConnectionPointPtr& get_thickness();

  /**
   * @brief Gets the tint color input component.
   */
  NodeMaterialConnectionPointPtr& get_tintColor();

  /**
   * @brief Gets the translucency intensity input component.
   */
  NodeMaterialConnectionPointPtr& get_translucencyIntensity();

  /**
   * @brief Gets the translucency diffusion distance input component.
   */
  NodeMaterialConnectionPointPtr& get_translucencyDiffusionDist();

  /**
   * @brief Gets the refraction object parameters.
   */
  NodeMaterialConnectionPointPtr& get_refraction();

  /**
   * @brief Gets the sub surface object output component.
   */
  NodeMaterialConnectionPointPtr& get_subsurface();

  /**
   * @brief Hidden
   */
  SubSurfaceBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the thickness component
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> thickness;

  /**
   * Gets the tint color input component
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> tintColor;

  /**
   * Gets the translucency intensity input component
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> translucencyIntensity;

  /**
   * Gets the translucency diffusion distance input component
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> translucencyDiffusionDist;

  /**
   * Gets the refraction object parameters
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> refraction;

  /**
   * Gets the sub surface object output component
   */
  ReadOnlyProperty<SubSurfaceBlock, NodeMaterialConnectionPointPtr> subsurface;

}; // end of class SubSurfaceBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_SUB_SURFACE_BLOCK_H
