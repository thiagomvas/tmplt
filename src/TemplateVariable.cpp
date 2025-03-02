#include "TemplateVariable.h"
#include "tmplt.h"
#include <sstream>
namespace tmplt {

TemplateVariable::TemplateVariable()
    : name(" "), description(" "), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name)
    : name(name), description(" "), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name,
                                   const std::string &description)
    : name(name), description(description), type(VariableType::Null) {}

TemplateVariable::TemplateVariable(const std::string &name,
                                   const std::string &description,
                                   const VariableType &type)
    : name(name), description(description), type(type) {}

std::string TemplateVariable::serialize() const {
  std::stringstream result;
  result << VARIABLE_HEADER << "\n";
  result << "name: " << name << "\n";
  result << "description: " << description << "\n";
  result << "type: " << std::to_string(static_cast<int>(type)) << "\n";
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

  // Ensure the first line is VARIABLE_HEADER
  if (!std::getline(stream, line) || line != VARIABLE_HEADER) {
    throw std::runtime_error("Invalid variable header: " + line);
  }

  // Read name
  if (std::getline(stream, line) && line.rfind("name: ", 0) == 0) {
    tv.name = line.substr(6);
  } else {
    throw std::runtime_error("Invalid or missing 'name' field");
  }

  // Read description
  if (std::getline(stream, line) && line.rfind("description: ", 0) == 0) {
    tv.description = line.substr(12);
  } else {
    throw std::runtime_error("Invalid or missing 'description' field");
  }

  // Read type
  if (std::getline(stream, line) && line.rfind("type: ", 0) == 0) {
    try {
      tv.type = static_cast<VariableType>(std::stoi(line.substr(6)));
    } catch (...) {
      throw std::runtime_error("Invalid 'type' value");
    }
  } else {
    throw std::runtime_error("Invalid or missing 'type' field");
  }

  // Read optional condition block
  if (std::getline(stream, line) && line == "condition:") {
    Condition condition;

    if (std::getline(stream, line) && line.rfind("  variableName: ", 0) == 0) {
      condition.variableName = line.substr(15);
    } else {
      throw std::runtime_error(
          "Invalid or missing 'variableName' in condition");
    }

    if (std::getline(stream, line) && line.rfind("  expectedValue: ", 0) == 0) {
      condition.expectedValue = line.substr(17);
    } else {
      throw std::runtime_error(
          "Invalid or missing 'expectedValue' in condition");
    }

    if (std::getline(stream, line) && line.rfind("  conditionType: ", 0) == 0) {
      try {
        condition.type = static_cast<ConditionType>(std::stoi(line.substr(17)));
      } catch (...) {
        throw std::runtime_error("Invalid 'conditionType' value");
      }
    } else {
      throw std::runtime_error(
          "Invalid or missing 'conditionType' in condition");
    }

    tv.condition = condition;
  }

  return tv;
}
} // namespace tmplt
