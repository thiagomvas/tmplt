#pragma once

#include "Condition.h"
#include "tmplt.h"
#include <optional>
#include <string>
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

  std::string serialize() const;
  static TemplateVariable deserialize(const std::string &buffer);
};
} // namespace tmplt
