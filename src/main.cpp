
#include "Condition.h"
#include "Template.h"
#include "TemplateEngine.h"
#include "TemplateFile.h"
#include "TemplateVariable.h"
#include "tmplt.h"
#include <iostream>
#include <ostream>

int main() {
  tmplt::TemplateEngine engine;
  auto tmpl = engine.createSingleFileTemplate("/home/thiagomv/test.txt");

  std::cout << tmpl.serialize() << std::endl;
  return 0;
}
