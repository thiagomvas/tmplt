#pragma once

#include "TemplateVariable.h"
#include <string>
#include <unordered_map>
namespace tmplt {
class TemplateEngine {
public:
  std::unordered_map<std::string, TemplateVariable>
  findVariablesInBuffer(const std::string &buffer);
};
} // namespace tmplt
