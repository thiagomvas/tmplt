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
  Template
  createMultipleFilesTemplate(const std::vector<std::string> &filePaths);

  void interactiveConfigureVariable(tmplt::TemplateVariable &variable);
  void interactiveConfigureFile(tmplt::TemplateFile &file);
};
} // namespace tmplt
