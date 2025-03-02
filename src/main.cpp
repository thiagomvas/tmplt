
#include "Condition.h"
#include "Template.h"
#include "TemplateEngine.h"
#include "TemplateFile.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <iostream>
#include <ostream>

int main() {
  tmplt::Template tmp;

  tmp.name = "Template Name";
  tmp.description = "Template Description";

  tmplt::TemplateEngine engine;
  auto vars = engine.findVariablesInBuffer(
      "Hello ${{Name}}$, You are ${{Age}}$ years old");

  std::vector<std::string> varNames;
  for (const auto &pair : vars) {
    varNames.push_back(pair.first);
  }

  tmplt::TemplateFile file;
  file.relativePath = "bar.txt";
  file.contentPath = ".config/bar.txt";
  file.variableNames = varNames;

  tmp.files.push_back(file);
  tmp.variables = vars;

  std::cout << tmplt::Template::deserialize(tmp.serialize()).serialize()
            << std::endl;

  return 0;
}
