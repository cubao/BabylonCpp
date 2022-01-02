#include <babylon/materials/node/blocks/vertex/instances_block.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

InstancesBlock::InstancesBlock(const std::string& iName)
    : NodeMaterialBlock(iName, NodeMaterialBlockTargets::Vertex)
    , world0{this, &InstancesBlock::get_world0}
    , world1{this, &InstancesBlock::get_world1}
    , world2{this, &InstancesBlock::get_world2}
    , world3{this, &InstancesBlock::get_world3}
    , world{this, &InstancesBlock::get_world}
    , output{this, &InstancesBlock::get_output}
    , instanceID{this, &InstancesBlock::get_instanceID}
{
  registerInput("world0", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("world1", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("world2", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("world3", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("world", NodeMaterialBlockConnectionPointTypes::Matrix, true);

  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Matrix);
  registerOutput("instanceID", NodeMaterialBlockConnectionPointTypes::Float);
}

InstancesBlock::~InstancesBlock() = default;

std::string InstancesBlock::getClassName() const
{
  return "InstancesBlock";
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_world0()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_world1()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_world2()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_world3()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_world()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_output()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& InstancesBlock::get_instanceID()
{
  return _outputs[1];
}

void InstancesBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!world0()->connectedPoint()) {
    auto world0Input = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "world0"; });

    if (!world0Input) {
      world0Input = InputBlock::New("world0");
      world0Input->setAsAttribute("world0");
    }
    world0Input->output()->connectTo(world0());
  }
  if (!world1()->connectedPoint()) {
    auto world1Input = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "world1"; });

    if (!world1Input) {
      world1Input = InputBlock::New("world1");
      world1Input->setAsAttribute("world1");
    }
    world1Input->output()->connectTo(world1);
  }
  if (!world2()->connectedPoint()) {
    auto world2Input = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "world2"; });

    if (!world2Input) {
      world2Input = InputBlock::New("world2");
      world2Input->setAsAttribute("world2");
    }
    world2Input->output()->connectTo(world2);
  }
  if (!world3()->connectedPoint()) {
    auto world3Input = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "world3"; });

    if (!world3Input) {
      world3Input = InputBlock::New("world3");
      world3Input->setAsAttribute("world3");
    }
    world3Input->output()->connectTo(world3);
  }
  if (!world()->connectedPoint()) {
    auto worldInput = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name() == "world"; });

    if (!worldInput) {
      worldInput = InputBlock::New("world");
      worldInput->setAsSystemValue(NodeMaterialSystemValues::World);
    }
    worldInput->output()->connectTo(world);
  }

  world()->define = "!INSTANCES || THIN_INSTANCES";
}

void InstancesBlock::prepareDefines(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                                    NodeMaterialDefines& defines, bool useInstances,
                                    SubMesh* subMesh)
{
  auto changed = false;
  if (defines["INSTANCES"] != useInstances) {
    defines.setValue("INSTANCES", useInstances);
    changed = true;
  }

  if (subMesh && defines["THIN_INSTANCES"] != subMesh->getRenderingMesh()->hasThinInstances()) {
    defines.setValue("THIN_INSTANCES", subMesh->getRenderingMesh()->hasThinInstances());
    changed = true;
  }

  if (changed) {
    defines.markAsUnprocessed();
  }
}

InstancesBlock& InstancesBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto engine = state.sharedData->scene->getEngine();

  // Register for defines
  state.sharedData->blocksWithDefines.emplace_back(this);

  // Emit code
  const auto& iOutput     = _outputs[0];
  const auto& iInstanceID = _outputs[1];
  const auto& iWorld0     = world0();
  const auto& iWorld1     = world1();
  const auto& iWorld2     = world2();
  const auto& iWorld3     = world3();

  state.compilationString += "#ifdef INSTANCES\r\n";
  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = mat4(%s, %s, %s, %s);\r\n",
                                                   iWorld0->associatedVariableName().c_str(),
                                                   iWorld1->associatedVariableName().c_str(),
                                                   iWorld2->associatedVariableName().c_str(),
                                                   iWorld3->associatedVariableName().c_str());
  state.compilationString += "#ifdef THIN_INSTANCES\r\n";
  state.compilationString += StringTools::printf(
    "%s = %s * %s;\r\n", output()->associatedVariableName().c_str(),
    world()->associatedVariableName().c_str(), output()->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
  if (engine->_caps.canUseGLInstanceID) {
    state.compilationString += _declareOutput(instanceID, state) + " = 0.0;\r\n";
  }
  else {
    state.compilationString += _declareOutput(iInstanceID, state) + " = float(gl_InstanceID);\r\n";
  }
  state.compilationString += "#else\r\n";
  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = %s;\r\n", world()->associatedVariableName().c_str());
  state.compilationString += _declareOutput(iInstanceID, state) + " = 0.0;\r\n";
  state.compilationString += "#endif\r\n";
  return *this;
}

} // end of namespace BABYLON
