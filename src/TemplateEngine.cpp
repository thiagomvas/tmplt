#include "TemplateEngine.h"
#include "Template.h"
#include "TemplateVariable.h"
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

namespace tmplt {

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

  return tmpl;
}

} // namespace tmplt
