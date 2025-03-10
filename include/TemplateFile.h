#pragma once

#include <string>
#include <vector>
namespace tmplt {
class TemplateFile {
public:
  std::string relativePath;
  std::string contentPath;
  std::vector<std::string> variableNames;
  std::string serialize() const;
  static TemplateFile deserialize(const std::string &buffer);
};
} // namespace tmplt
