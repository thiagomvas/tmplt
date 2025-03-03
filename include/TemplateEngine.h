#pragma once

#include "Template.h"
#include "TemplateVariable.h"
#include <string>
#include <unordered_map>
namespace tmplt {
class TemplateEngine {
public:
  std::unordered_map<std::string, TemplateVariable>
  findVariablesInBuffer(const std::string &buffer);

  Template createSingleFileTemplate(const std::string &filePath);

  void interactiveConfigureVariable(tmplt::TemplateVariable &variable);
};
} // namespace tmplt
