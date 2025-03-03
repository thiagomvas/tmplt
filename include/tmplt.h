#pragma once

#include <string>
namespace tmplt {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";
const std::string MAGENTA = "\033[35m";

const std::string VARIABLE_HEADER = "[VARIABLE]";
const std::string FILE_HEADER = "[FILE]";

enum VariableType { Null, Text, Bool, Enum };
enum ConditionType { Equals, NotEquals };

} // namespace tmplt
