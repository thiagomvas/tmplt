
#include "Condition.h"
#include "TemplateEngine.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <iostream>
#include <ostream>

int main() {
  tmplt::TemplateEngine engine;
  std::string buffer = "Hello ${{Name}}$, you are ${{Age}}$ years old";

  auto vars = engine.findVariablesInBuffer(buffer);

  // Print out the found variables
  std::cout << "Variables found in the buffer:\n";
  for (const auto &pair : vars) {
    std::cout << "Variable name: " << pair.first << std::endl;
  }
}
