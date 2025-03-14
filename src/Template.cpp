#include "Template.h"
#include "tmplt.h"
#include <ostream>
#include <sstream>

namespace tmplt {
std::string Template::serialize() const {
  std::ostringstream out;

  out << "name:" << name << std::endl
      << "description:" << description << std::endl;

  // Variables
  for (const auto &pair : variables) {
    out << pair.second.serialize() << std::endl;
  }

  // Files
  for (const auto &file : files) {
    out << file.serialize() << std::endl;
  }

  return out.str();
}

Template Template::deserialize(const std::string &buffer) {
  Template tmpl;
  std::istringstream stream(buffer);
  std::string line;

  // Read name
  if (!std::getline(stream, line) || line.rfind("name:", 0) != 0) {
    throw std::runtime_error("Invalid or missing 'name' field");
  }
  tmpl.name = line.substr(5); // Extract name

  // Read description
  if (!std::getline(stream, line) || line.rfind("description:", 0) != 0) {
    throw std::runtime_error("Invalid or missing 'description' field");
  }
  tmpl.description = line.substr(12); // Extract description

  // Read variables and files
  while (std::getline(stream, line)) {
    if (line == VARIABLE_HEADER) {
      std::string variableData = line + "\n";
      while (std::getline(stream, line) && !line.empty()) {
        variableData += line + "\n";
      }
      try {
        TemplateVariable var = TemplateVariable::deserialize(variableData);
        if (var.name.empty()) {
          throw std::runtime_error("Variable missing name");
        }
        tmpl.variables[var.name] = var;
      } catch (const std::exception &e) {
        throw std::runtime_error("Failed to deserialize variable: " +
                                 std::string(e.what()));
      }
    } else if (line == FILE_HEADER) {
      std::string fileData = line + "\n";
      while (std::getline(stream, line) && !line.empty()) {
        fileData += line + "\n";
      }
      try {
        tmplt::TemplateFile file = tmplt::TemplateFile::deserialize(fileData);
        if (file.relativePath.empty()) {
          throw std::runtime_error("File missing relativePath");
        }
        tmpl.files.push_back(file); // Add file to the template
      } catch (const std::exception &e) {
        throw std::runtime_error("Failed to deserialize file: " +
                                 std::string(e.what()));
      }
    } else {
      throw std::runtime_error("Unexpected content in template: " + line);
    }
  }

  return tmpl;
}

} // namespace tmplt
