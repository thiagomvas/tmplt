#pragma once

#include "TemplateFile.h"
#include "TemplateVariable.h"
#include <string>
#include <unordered_map>
#include <vector>
namespace tmplt {
class Template {
public:
  std::string name;
  std::string description;
  std::unordered_map<std::string, TemplateVariable> variables;
  std::vector<TemplateFile> files;

  std::string serialize() const;
  static Template deserialize(const std::string &buffer);
};
} // namespace tmplt
