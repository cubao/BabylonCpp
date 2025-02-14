#ifndef BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_TARGETS_H
#define BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_TARGETS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum used to define the target of a block.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialBlockTargets : unsigned int {
  /** Vertex shader */
  Vertex = 1,
  /** Fragment shader */
  Fragment = 2,
  /** Neutral */
  Neutral = 4,
  /** Vertex and Fragment */
  VertexAndFragment = Vertex | Fragment,
  /** Target is undefined */
  Undefined = 5,
}; // end of enum class NodeMaterialBlockTargets

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_TARGETS_H
