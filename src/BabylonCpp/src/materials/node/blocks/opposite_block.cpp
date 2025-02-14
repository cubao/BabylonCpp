#include <babylon/materials/node/blocks/opposite_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

OppositeBlock::OppositeBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &OppositeBlock::get_input}
    , output{this, &OppositeBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

OppositeBlock::~OppositeBlock() = default;

std::string OppositeBlock::getClassName() const
{
  return "OppositeBlock";
}

NodeMaterialConnectionPointPtr& OppositeBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& OppositeBlock::get_output()
{
  return _outputs[0];
}

OppositeBlock& OppositeBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = 1. - %s;\r\n", input()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
