#pragma once

#include "Condition.h"
#include "tmplt.h"
#include <optional>
#include <string>
#include <unordered_map>
namespace tmplt {
class TemplateVariable {
public:
  TemplateVariable();
  TemplateVariable(const std::string &name);
  TemplateVariable(const std::string &name, const std::string &description);
  TemplateVariable(const std::string &name, const std::string &description,
                   const VariableType &type);
  std::string name;
  std::string description;

  VariableType type;
  std::optional<Condition> condition;

  std::optional<std::string> defaultValue;

  std::optional<std::string> trueValue;
  std::optional<std::string> falseValue;

  std::unordered_map<std::string, std::string> enumMap;

  std::string serialize() const;
  static TemplateVariable deserialize(const std::string &buffer);
};
} // namespace tmplt
