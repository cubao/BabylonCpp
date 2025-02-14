#include <babylon/meshes/builders/cylinder_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr CylinderBuilder::CreateCylinder(const std::string& name, CylinderOptions& options,
                                        Scene* scene)
{
  const auto cylinder = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  cylinder->_originalBuilderSideOrientation = *options.sideOrientation;

  const auto vertexData = VertexData::CreateCylinder(options);

  vertexData->applyToMesh(*cylinder, options.updatable);

  return cylinder;
}

} // end of namespace BABYLON
