#pragma once

#include "tmplt.h"
#include <string>
namespace tmplt {
class Condition {
public:
  std::string variableName;
  std::string expectedValue;
  ConditionType type;
};

} // namespace tmplt
