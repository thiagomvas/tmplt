#include "Template.h"
#include "TemplateEngine.h"
#include "cli.h"
#include "tmplt.h"
#include <filesystem>
#include <iostream>
#include <optional>

namespace fs = std::filesystem;
void handleGenerate(const CLI::ParsedArgs &args) {
  std::string templateName = args.arguments[0];

  tmplt::TemplateEngine engine;
  auto tmpl = engine.createSingleFileTemplate("/home/thiagomv/test.txt");

  if (args.options.count("interactive") > 0) {
    std::cout << "Creating template interactively" << std::endl;
    return;
  }

  for (int i = 1; i < args.arguments.size(); i++) {
    std::string arg = args.arguments[i];
    size_t pos = arg.find("=");

    if (pos == std::string::npos)
      continue;

    std::string name = arg.substr(0, pos);
    std::string value = arg.substr(pos + 1);

    if (tmpl.variables.contains(name))
      std::cout << "Set '" << name << "' to '" << value << "'" << std::endl;
  }
}

void processArguments(const CLI::ParsedArgs &args, bool &hasFile,
                      bool &hasDirectory) {
  for (const auto &arg : args.arguments) {
    fs::path path(arg);
    if (!fs::exists(path)) {
      std::cout << "Path does not exist: " << arg << std::endl;
      return;
    }
    if (fs::is_regular_file(path)) {
      hasFile = true;
    } else if (fs::is_directory(path)) {
      hasDirectory = true;
    } else {
      std::cout << "Unknown type: " << arg << std::endl;
      return;
    }
  }
}

std::optional<tmplt::Template> createTemplate(const CLI::ParsedArgs &args,
                                              tmplt::TemplateEngine &engine,
                                              bool hasFile, bool hasDirectory) {
  if (hasFile && hasDirectory) {
    std::cout << "Error: Cannot mix files and directories in the arguments."
              << std::endl;
    return std::nullopt;
  }
  if (hasFile) {
    return (args.arguments.size() == 1)
               ? engine.createSingleFileTemplate(args.arguments[0])
               : engine.createMultipleFilesTemplate(args.arguments);
  } else if (hasDirectory) {
    if (args.arguments.size() == 1) {
      std::cout << "One directory provided: " << args.arguments[0] << std::endl;
    } else {
      std::cout << "Error: Only one directory is allowed." << std::endl;
      return std::nullopt;
    }
  }
  std::cout << "Error: No valid files or directories provided." << std::endl;
  return std::nullopt;
}

void configureTemplate(tmplt::Template &tmpl, tmplt::TemplateEngine &engine) {
  std::string input;
  std::cout << tmplt::MAGENTA << "Template Name:" << tmplt::RESET;
  std::getline(std::cin, input);
  tmpl.name = input;

  std::cout << tmplt::MAGENTA << "Template description: " << tmplt::RESET;
  std::getline(std::cin, input);
  tmpl.description = input;

  for (auto &var : tmpl.variables) {
    engine.interactiveConfigureVariable(var.second);
  }
  for (auto &file : tmpl.files) {
    engine.interactiveConfigureFile(file);

    auto variables = engine.findVariablesInBuffer(file.relativePath);
    for (auto &var : variables) {
      if (!tmpl.variables.contains(var.first)) {
        engine.interactiveConfigureVariable(var.second);
        tmpl.variables.insert(var);
      }
    }
  }
}

void handleCreate(const CLI::ParsedArgs &args) {
  bool hasFile = false, hasDirectory = false;
  tmplt::TemplateEngine engine;

  processArguments(args, hasFile, hasDirectory);
  auto tmpl = createTemplate(args, engine, hasFile, hasDirectory);

  if (!tmpl)
    return;

  configureTemplate(*tmpl, engine);

  std::cout << tmplt::GREEN << "Template created successfully: " << tmplt::RESET
            << std::endl;
  std::cout << tmpl->serialize() << std::endl;
}

int main(int argc, char *argv[]) {
  // Register commands

  CLI::registerCommand("generate", "Generate new files from a template");
  CLI::registerArgument("generate", "template", "The template to generate");
  CLI::registerOption("generate", "interactive", CLI::OptionType::Boolean,
                      "Generate the template step by step via the terminal",
                      "i");

  CLI::registerCommand("create",
                       "Create a new template from files or a directory");

  try {
    auto args = CLI::parse(argc, argv);

    if (args.command == "generate")
      handleGenerate(args);
    else if (args.command == "create")
      handleCreate(args);

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
