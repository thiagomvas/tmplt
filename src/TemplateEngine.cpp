#include "TemplateEngine.h"
#include "TemplateVariable.h"

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
} // namespace tmplt
