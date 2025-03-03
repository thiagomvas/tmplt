
#pragma once
#include "Template.h"
#include "TemplateEngine.h"
#include <string>
#include <vector>

class CLI {
public:
  CLI(tmplt::TemplateEngine &engine);

  void printUsage() const;
  void run(int argc, char *argv[]);

private:
  tmplt::TemplateEngine &engine;

  void handleCreate(const std::vector<std::string> &args);
  void handleApply(const std::vector<std::string> &args);
  void handleList();
  void configureVariable(tmplt::TemplateVariable &variable);
};
