#ifndef BABYLON_MESHES_BUILDERS_SHAPE_BUILDER_H
#define BABYLON_MESHES_BUILDERS_SHAPE_BUILDER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/vector4.h>

namespace BABYLON {

class ExtrudeShapeOptions;
class ExtrudeShapeCustomOptions;
class Scene;
FWD_CLASS_SPTR(Mesh)

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
class BABYLON_SHARED_EXPORT ShapeBuilder {

public:
  // clang-format off
  /**
   * @brief Creates an extruded shape mesh. The extrusion is a parametric shape. It has no predefined shape. Its final shape will depend on the input parameters.
   * * The parameter `shape` is a required array of successive Vector3. This array depicts the shape to be extruded in its local space : the shape must be designed in the xOy plane and will be extruded along the Z axis.
   * * The parameter `path` is a required array of successive Vector3. This is the axis curve the shape is extruded along.
   * * The parameter `rotation` (float, default 0 radians) is the angle value to rotate the shape each step (each path point), from the former step (so rotation added each step) along the curve.
   * * The parameter `scale` (float, default 1) is the value to scale the shape.
   * * The parameter `cap` sets the way the extruded shape is capped. Possible values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START, BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * * The optional parameter `instance` is an instance of an existing ExtrudedShape object to be updated with the passed `shape`, `path`, `scale` or `rotation` parameters : https://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh#extruded-shape
   * * Remember you can only change the shape or path point positions, not their number when updating an extruded shape.
   * * You can also set the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the texture image to crop and stick respectively on the front and the back sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here : https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The optional parameter `invertUV` (boolean, default false) swaps in the geometry the U and V coordinates to apply a texture.
   * * The mesh can be set to updatable with the boolean parameter `updatable` (default false) if its internal geometry is supposed to change once created.
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the extruded shape mesh
   * @see https://doc.babylonjs.com/how_to/parametric_shapes
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes
   */
  // clang-format on
  static MeshPtr ExtrudeShape(const std::string& name, ExtrudeShapeOptions& options,
                              Scene* scene = nullptr);

  // clang-format off
  /**
   * @brief Creates an custom extruded shape mesh.
   * The custom extrusion is a parametric shape. It has no predefined shape. Its final shape will depend on the input parameters.
   * * The parameter `shape` is a required array of successive Vector3. This array depicts the shape to be extruded in its local space : the shape must be designed in the xOy plane and will be extruded along the Z axis.
   * * The parameter `path` is a required array of successive Vector3. This is the axis curve the shape is extruded along.
   * * The parameter `rotationFunction` (JS function) is a custom Javascript function called on each path point. This function is passed the position i of the point in the path and the distance of this point from the beginning of the path
   * * It must returns a float value that will be the rotation in radians applied to the shape on each path point.
   * * The parameter `scaleFunction` (JS function) is a custom Javascript function called on each path point. This function is passed the position i of the point in the path and the distance of this point from the beginning of the path
   * * It must returns a float value that will be the scale value applied to the shape on each path point
   * * The parameter `ribbonClosePath` (boolean, default false) forces the extrusion underlying ribbon to close all the paths in its `pathArray`
   * * The parameter `ribbonCloseArray` (boolean, default false) forces the extrusion underlying ribbon to close its `pathArray`
   * * The parameter `cap` sets the way the extruded shape is capped. Possible values : BABYLON.Mesh.NO_CAP (default), BABYLON.Mesh.CAP_START, BABYLON.Mesh.CAP_END, BABYLON.Mesh.CAP_ALL
   * * The optional parameter `instance` is an instance of an existing ExtrudedShape object to be updated with the passed `shape`, `path`, `scale` or `rotation` parameters : https://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh#extruded-shape
   * * Remember you can only change the shape or path point positions, not their number when updating an extruded shape
   * * You can also set the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the texture image to crop and stick respectively on the front and the back sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here : https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The optional parameter `invertUV` (boolean, default false) swaps in the geometry the U and V coordinates to apply a texture
   * * The mesh can be set to updatable with the boolean parameter `updatable` (default false) if its internal geometry is supposed to change once created
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the custom extruded shape mesh
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#custom-extruded-shapes
   * @see https://doc.babylonjs.com/how_to/parametric_shapes
   * @see https://doc.babylonjs.com/how_to/parametric_shapes#extruded-shapes
   */
  // clang-format on
  static MeshPtr ExtrudeShapeCustom(const std::string& name, ExtrudeShapeCustomOptions& options,
                                    Scene* scene = nullptr);

private:
  static MeshPtr _ExtrudeShapeGeneric(
    const std::string& name, const std::vector<Vector3>& shape, const std::vector<Vector3>& curve,
    const std::optional<float>& scale, const std::optional<float>& rotation,
    const std::function<float(float i, float distance)>& scaleFunction,
    const std::function<float(float i, float distance)>& rotateFunction, bool rbCA, bool rbCP,
    unsigned int cap, bool custom, Scene* scene, bool updtbl, unsigned int side, MeshPtr instance,
    bool invertUV, const std::optional<Vector4>& frontUVs, const std::optional<Vector4>& backUVs);

}; // end of class ShapeBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_SHAPE_BUILDER_H
