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

void handleCreate(const CLI::ParsedArgs &args) {
  bool hasFile = false;
  bool hasDirectory = false;
  tmplt::TemplateEngine engine;
  std::optional<tmplt::Template> tmpl;

  // Loop through each argument to check if it's a file or directory
  for (const auto &arg : args.arguments) {
    fs::path path(arg);

    if (fs::exists(path)) {
      if (fs::is_regular_file(path)) {
        // It's a file
        hasFile = true;
      } else if (fs::is_directory(path)) {
        // It's a directory
        hasDirectory = true;
      } else {
        std::cout << "Unknown type: " << arg << std::endl;
        return;
      }
    } else {
      std::cout << "Path does not exist: " << arg << std::endl;
      return;
    }
  }

  // Check validation conditions
  if (hasFile && hasDirectory) {
    std::cout << "Error: Cannot mix files and directories in the arguments."
              << std::endl;
    return;
  }

  if (hasFile) {
    if (args.arguments.size() == 1) {
      tmpl = engine.createSingleFileTemplate(args.arguments[0]);
    } else {
      // Multiple files
      std::cout << "Multiple files provided: ";
      for (const auto &file : args.arguments) {
        std::cout << file << " ";
      }
      std::cout << std::endl;
    }
  } else if (hasDirectory) {
    if (args.arguments.size() == 1) {
      // One directory
      std::cout << "One directory provided: " << args.arguments[0] << std::endl;
    } else {
      std::cout << "Error: Only one directory is allowed." << std::endl;
    }
  } else {
    std::cout << "Error: No valid files or directories provided." << std::endl;
  }

  std::string input;

  std::cout << tmplt::MAGENTA << "Template Name:" << tmplt::RESET;
  std::getline(std::cin, input);
  tmpl->name = input;

  std::cout << tmplt::MAGENTA << "Template description: " << tmplt::RESET;
  std::getline(std::cin, input);
  tmpl->description = input;

  for (auto &var : tmpl->variables) {
    engine.interactiveConfigureVariable(var.second);
  }

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
