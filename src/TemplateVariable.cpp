#include "TemplateVariable.h"
#include "tmplt.h"
#include <sstream>
namespace tmplt {

TemplateVariable::TemplateVariable()
    : name(""), description(""), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name)
    : name(name), description(""), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name,
                                   const std::string &description)
    : name(name), description(description), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name,
                                   const std::string &description,
                                   const VariableType &type)
    : name(name), description(description), type(type) {}

std::string TemplateVariable::serialize() const {
  std::stringstream result;
  result << VARIABLE_HEADER << "\n"; // Use the constant header
  result << "name: " << name << "\n";
  result << "description: " << description << "\n";
  result << "type: " << std::to_string(static_cast<int>(type))
         << "\n"; // Assuming type can be cast to int
  if (condition) {
    result << "condition:\n";
    result << "  variableName: " << condition->variableName << "\n";
    result << "  expectedValue: " << condition->expectedValue << "\n";
    result << "  conditionType: "
           << std::to_string(static_cast<int>(condition->type)) << "\n";
  }
  return result.str();
}

TemplateVariable TemplateVariable::deserialize(const std::string &buffer) {
  TemplateVariable tv;
  std::istringstream stream(buffer);
  std::string line;

  // Read name, description, type
  std::getline(stream, line); // skip VARIABLE_HEADER

  std::getline(stream, line);
  tv.name = line.substr(6); // "name: <name>"

  std::getline(stream, line);
  tv.description = line.substr(12); // "description: <description>"

  std::getline(stream, line);
  tv.type =
      static_cast<VariableType>(std::stoi(line.substr(6))); // "type: <type>"

  // Read optional condition block
  if (std::getline(stream, line) &&
      line.find("condition:") != std::string::npos) {
    Condition condition;
    std::getline(stream, line);
    condition.variableName = line.substr(15); // "  variableName: <name>"
    std::getline(stream, line);
    condition.expectedValue = line.substr(17); // "  expectedValue: <value>"
    std::getline(stream, line);
    condition.type = static_cast<ConditionType>(
        std::stoi(line.substr(17))); // "  conditionType: <type>"
    tv.condition = condition;
  }

  return tv;
}
} // namespace tmplt
