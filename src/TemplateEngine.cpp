#include "TemplateEngine.h"
#include "Template.h"
#include "TemplateFile.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;
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

void TemplateEngine::interactiveGenerateTemplate(
    const tmplt::Template &tmpl, const std::string &targetPath) {

  // Configure variable values
  std::unordered_map<std::string, std::string> varValueMap;

  for (const auto &var : tmpl.variables) {
    std::string buffer;
    switch (var.second.type) {
    case tmplt::VariableType::Text:
      std::cout << CYAN << "Enter value for '" << var.second.name
                << "' (default: '"
                << (var.second.defaultValue.value_or("Not Set"))
                << "'): " << RESET;
      std::getline(std::cin, buffer);

      // Use default if input is empty
      if (buffer.empty()) {
        buffer = var.second.defaultValue.value_or("Not Set");
      }

      varValueMap[var.second.name] = buffer;
      break;

    case tmplt::VariableType::Bool:
      std::cout << CYAN << "Enter value for '" << var.second.name
                << "' (true/false, default: '"
                << (var.second.trueValue.value_or("Not Set"))
                << "'): " << RESET;
      std::getline(std::cin, buffer);

      // Check for empty input and use default if necessary
      if (buffer.empty()) {
        buffer =
            var.second.trueValue.value_or("false"); // Use 'false' as a fallback
      }

      // Normalize to lower case for comparison
      std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
      if (buffer == "true" || buffer == "t" || buffer == "1") {
        varValueMap[var.second.name] = var.second.trueValue.value_or("");
      } else if (buffer == "false" || buffer == "f" || buffer == "0") {
        varValueMap[var.second.name] = var.second.falseValue.value_or("");
      } else {
        std::cout << RED << "Invalid value. Expected 'true/t/1' or 'false/f/0'."
                  << RESET << "\n";
        continue;
      }
      break;

    case tmplt::VariableType::Enum:
      std::cout << CYAN << "Enter value for '" << var.second.name
                << "' (choose from available options): " << RESET;

      // Display enum map options
      if (!var.second.enumMap.empty()) {
        std::cout << CYAN << "Available options: " << RESET;
        for (const auto &enumPair : var.second.enumMap) {
          std::cout << "'" << enumPair.first << "' -> " << enumPair.second
                    << ", ";
        }
        std::cout << "\n";
      }

      std::getline(std::cin, buffer);

      // If empty, use the first available value in the enumMap
      if (buffer.empty() && !var.second.enumMap.empty()) {
        buffer =
            var.second.enumMap.begin()->first; // Default to the first enum key
      }

      // Validate the user input against the available enum keys
      if (var.second.enumMap.find(buffer) != var.second.enumMap.end()) {
        varValueMap[var.second.name] = var.second.enumMap.find(buffer)->second;
      } else {
        std::cout
            << RED
            << "Invalid option. Please choose a valid option from the list."
            << RESET << "\n";
        continue; // If invalid, prompt again
      }
      break;

    default:
      std::cout << RED << "Unsupported variable type." << RESET << "\n";
      break;
    }
  }

  // Log mappings
  for (const auto &pair : varValueMap) {
    std::cout << pair.first << "=" << pair.second << std::endl;
  }

  // Create files
  generateFiles(tmpl, targetPath, varValueMap);

  std::cout << CYAN << "Template generated at: " << targetPath << RESET << "\n";
}

void TemplateEngine::generateFiles(
    const tmplt::Template &tmpl, const std::string &targetPath,
    const std::unordered_map<std::string, std::string> &variableReplacements) {

  // Iterate through all files in the template
  for (const auto &file : tmpl.files) {
    std::string relativePath = file.relativePath;

    // Replace variables in the file path using the provided mapping
    for (const auto &var : variableReplacements) {
      std::string varPattern = "${{" + var.first + "}}$";
      size_t pos = 0;

      // Replace occurrences of the variable in the file path with the
      // replacement value
      while ((pos = relativePath.find(varPattern, pos)) != std::string::npos) {
        relativePath.replace(pos, varPattern.length(), var.second);
        pos += var.second.length();
      }
    }

    // Generate the full file path by combining targetPath and relativePath
    fs::path fullFilePath = fs::path(targetPath) / relativePath;

    // Ensure the directory exists
    fs::create_directories(fullFilePath.parent_path());

    // Read the content from the source file
    std::ifstream sourceFile(file.contentPath);
    if (!sourceFile.is_open()) {
      std::cerr << RED << "Error opening source file: " << file.contentPath
                << RESET << "\n";
      continue;
    }

    std::stringstream contentStream;
    contentStream << sourceFile.rdbuf();
    std::string fileContent = contentStream.str();

    // Replace variables in the content of the file using the provided mapping
    for (const auto &var : variableReplacements) {
      std::string varPattern = "${{" + var.first + "}}$";
      size_t pos = 0;

      // Replace occurrences of the variable in the content with the replacement
      // value
      while ((pos = fileContent.find(varPattern, pos)) != std::string::npos) {
        fileContent.replace(pos, varPattern.length(), var.second);
        pos += var.second.length();
      }
    }

    // Write the content to the full file path
    std::ofstream outFile(fullFilePath);
    if (outFile.is_open()) {
      outFile << fileContent; // Write the modified content to the file
      outFile.close();
      std::cout << CYAN << "Created file at: " << fullFilePath << RESET << "\n";
    } else {
      std::cerr << RED << "Error creating file at: " << fullFilePath << RESET
                << "\n";
    }
  }
}
} // namespace tmplt
