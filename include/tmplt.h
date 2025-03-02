#pragma once

#include <string>
namespace tmplt {

const std::string VARIABLE_HEADER = "[VARIABLE]";

enum VariableType { Null, Text, Bool, Enum };
enum ConditionType { Equals, NotEquals };
} // namespace tmplt
