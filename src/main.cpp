
#include "Condition.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <iostream>
#include <ostream>

int main() {
  tmplt::TemplateVariable var("Test", "Some Test Variable",
                              tmplt::VariableType::Bool);

  tmplt::Condition con;
  con.expectedValue = "Foobar";
  con.variableName = "Foo";
  con.type = tmplt::ConditionType::NotEquals;

  var.condition = con;

  auto des = tmplt::TemplateVariable::deserialize(var.serialize());

  std::cout << des.serialize() << std::endl;

  return 0;
}
