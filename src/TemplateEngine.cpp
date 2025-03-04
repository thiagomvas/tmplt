#include "TemplateEngine.h"
#include "Template.h"
#include "TemplateFile.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
namespace tmplt {
std::vector<std::string>
extractKeys(const std::unordered_map<std::string, TemplateVariable> &map) {
  std::vector<std::string> keys;

  // Iterate through the unordered_map and extract keys
  for (const auto &pair : map) {
    keys.push_back(pair.first); // Add the key to the vector
  }

  return keys;
}
std::unordered_map<std::string, TemplateVariable>
TemplateEngine::findVariablesInBuffer(const std::string &buffer) {
  std::unordered_map<std::string, TemplateVariable> variables;
  size_t pos = 0;

  // Iterate through the buffer and look for the pattern ${{variable}}$
  while ((pos = buffer.find("${{", pos)) != std::string::npos) {
    // Find the closing }}$
    size_t endPos = buffer.find("}}$", pos);
    if (endPos == std::string::npos) {
      break; // No closing }}$, stop processing
    }

    std::string varName = buffer.substr(pos + 3, endPos - pos - 3);

    TemplateVariable tv(varName);
    variables[varName] = tv;

    pos = endPos + 3;
  }

  return variables;
}

Template TemplateEngine::createSingleFileTemplate(const std::string &filePath) {
  Template tmpl;

  // Open the file in binary mode to check its contents
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  std::vector<char> buffer(512);
  file.read(buffer.data(), buffer.size());
  std::streamsize bytesRead = file.gcount();

  for (std::streamsize i = 0; i < bytesRead; ++i) {
    if (static_cast<unsigned char>(buffer[i]) < 9 || buffer[i] == 127) {
      return tmpl; // Return an empty template if binary
    }
  }

  file.clear();
  file.seekg(0, std::ios::beg);

  std::ostringstream contentStream;
  contentStream << file.rdbuf();
  std::string fileContents = contentStream.str();

  tmpl.name = "Unnamed";
  tmpl.description = "Undescription";
  auto vars = findVariablesInBuffer(fileContents);
  tmpl.variables = vars;

  TemplateFile f;
  f.variableNames = extractKeys(vars);
  f.contentPath = filePath;
  tmpl.files.push_back(f);

  return tmpl;
}
Template TemplateEngine::createMultipleFilesTemplate(
    const std::vector<std::string> &filePaths) {
  Template tmpl;
  std::vector<std::string> fileContentsList;

  for (const auto &filePath : filePaths) {
    // Open the file in binary mode to check its contents
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::vector<char> buffer(512);
    file.read(buffer.data(), buffer.size());
    std::streamsize bytesRead = file.gcount();

    bool isBinary = false;
    for (std::streamsize i = 0; i < bytesRead; ++i) {
      if (static_cast<unsigned char>(buffer[i]) < 9 || buffer[i] == 127) {
        isBinary = true; // Detect binary file
        break;
      }
    }

    if (!isBinary) {
      // File is not binary, read it fully
      file.clear();
      file.seekg(0, std::ios::beg);
      std::ostringstream contentStream;
      contentStream << file.rdbuf();
      auto vars = findVariablesInBuffer(contentStream.str());
      for (const auto &var : vars)
        tmpl.variables.insert(var);

      TemplateFile f;
      f.relativePath = filePath;
      f.variableNames = extractKeys(vars);
      tmpl.files.push_back(f);
    }
  }

  // Set template name and description
  tmpl.name = "Unnamed";
  tmpl.description = "Undescription";

  return tmpl;
}

void TemplateEngine::interactiveConfigureVariable(TemplateVariable &variable) {
  std::cout << CYAN << "Configuring variable: " << variable.name << RESET
            << "\n";

  // Modify description
  std::string newDescription;
  std::cout << CYAN << "Current description: " << variable.description << "\n";
  std::cout << CYAN
            << "Enter new description (leave empty to keep current): " << RESET;
  std::getline(std::cin, newDescription);
  if (!newDescription.empty()) {
    variable.description = newDescription;
  }

  // Modify type
  std::cout << CYAN << "Current type: " << static_cast<int>(variable.type)
            << "\n";
  std::cout << CYAN << "Enter new type (1: Text, 2: Bool, 3: Enum): " << RESET;
  int newType;
  std::cin >> newType;
  std::cin.ignore(); // to discard the newline character

  if (newType > 0 && newType <= 3) {
    variable.type = static_cast<tmplt::VariableType>(newType);
  } else {
    std::cout << RED << "Invalid type. Keeping current type." << RESET << "\n";
  }

  // Configure type-specific values
  switch (variable.type) {
  case tmplt::VariableType::Bool: {
    // Modify trueValue
    std::string newTrueValue;
    std::cout << CYAN << "Current trueValue: "
              << (variable.trueValue ? *variable.trueValue : "Not Set") << "\n";
    std::cout << CYAN
              << "Enter new trueValue (leave empty to keep current): " << RESET;
    std::getline(std::cin, newTrueValue);
    if (!newTrueValue.empty()) {
      variable.trueValue = newTrueValue;
    }

    // Modify falseValue
    std::string newFalseValue;
    std::cout << CYAN << "Current falseValue: "
              << (variable.falseValue ? *variable.falseValue : "Not Set")
              << "\n";
    std::cout << CYAN << "Enter new falseValue (leave empty to keep current): "
              << RESET;
    std::getline(std::cin, newFalseValue);
    if (!newFalseValue.empty()) {
      variable.falseValue = newFalseValue;
    }
    break;
  }

  case tmplt::VariableType::Text: {
    // Modify defaultValue
    std::string newDefaultValue;
    std::cout << CYAN << "Current defaultValue: "
              << (variable.defaultValue ? *variable.defaultValue : "Not Set")
              << "\n";
    std::cout << CYAN
              << "Enter new defaultValue (leave empty to keep current): "
              << RESET;
    std::getline(std::cin, newDefaultValue);
    if (!newDefaultValue.empty()) {
      variable.defaultValue = newDefaultValue;
    }
    break;
  }

  case tmplt::VariableType::Enum: {
    // Modify enumMap
    std::string enumKey, enumValue;
    char addMore = 'y';

    while (addMore == 'y' || addMore == 'Y') {
      std::cout << CYAN << "Enter enum key (leave empty to stop): " << RESET;
      std::getline(std::cin, enumKey);

      if (enumKey.empty()) {
        break;
      }

      std::cout << CYAN << "Enter enum value: " << RESET;
      std::getline(std::cin, enumValue);

      variable.enumMap[enumKey] = enumValue;

      std::cout << CYAN << "Add another enum mapping? (y/n): " << RESET;
      std::cin >> addMore;
      std::cin.ignore(); // to discard the newline character
    }
    break;
  }

  default:
    std::cout << RED << "Unsupported type. No modifications made." << RESET
              << "\n";
    break;
  }

  std::cout << CYAN << "Configuration updated for variable: " << variable.name
            << RESET << "\n";
}

void TemplateEngine::interactiveConfigureFile(TemplateFile &file) {
  std::cout
      << CYAN
      << "Enter the desired relative file path for the generated copy of '"
      << file.contentPath << "': ";
  std::getline(std::cin, file.relativePath);
}

} // namespace tmplt
