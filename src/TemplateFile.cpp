#include "TemplateFile.h"
#include "tmplt.h"

#include <sstream>

namespace tmplt {

std::string TemplateFile::serialize() const {
  std::stringstream result;
  result << FILE_HEADER << "\n";
  result << "relativePath: " << relativePath << "\n";
  result << "contentPath: " << contentPath << "\n";

  // Serialize the variable names
  if (!variableNames.empty()) {
    result << "variableNames:\n";
    for (const auto &varName : variableNames) {
      result << "  - " << varName << "\n";
    }
  }

  return result.str();
}

TemplateFile TemplateFile::deserialize(const std::string &buffer) {
  TemplateFile tf;
  std::istringstream stream(buffer);
  std::string line;

  // Ensure the first line is "file_header"
  if (!std::getline(stream, line) || line != FILE_HEADER) {
    throw std::runtime_error("Invalid file header: " + line);
  }

  // Read relativePath
  if (std::getline(stream, line) && line.rfind("relativePath: ", 0) == 0) {
    tf.relativePath = line.substr(14);
  } else {
    throw std::runtime_error("Invalid or missing 'relativePath' field");
  }

  // Read contentPath
  if (std::getline(stream, line) && line.rfind("contentPath: ", 0) == 0) {
    tf.contentPath = line.substr(13);
  } else {
    throw std::runtime_error("Invalid or missing 'contentPath' field");
  }

  // Read variableNames block
  if (std::getline(stream, line) && line == "variableNames:") {
    while (std::getline(stream, line)) {
      if (line.empty())
        break; // Stop if there's an empty line (end of block)
      if (line.rfind("  - ", 0) == 0) {
        tf.variableNames.push_back(line.substr(4)); // Extract variable name
      } else {
        throw std::runtime_error("Invalid format in 'variableNames' block");
      }
    }
  }

  return tf;
}
} // namespace tmplt
