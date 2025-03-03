
#include "TemplateEngine.h"

#include "TemplateEngine.h"
#include "cli.h"

int main(int argc, char *argv[]) {
  tmplt::TemplateEngine engine;
  CLI cli(engine);
  cli.run(argc, argv);
  return 0;
}
