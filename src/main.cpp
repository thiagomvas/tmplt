#include "Template.h"
#include "TemplateEngine.h"
#include "cli.h"
#include "tmplt.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

namespace fs = std::filesystem;

fs::path getAppDataPath() {
#ifdef _WIN32
  const char *appData = std::getenv("APPDATA");
  if (appData) {
    return fs::path(appData);
  }
#else
  const char *home = std::getenv("HOME");
  if (home) {
    return fs::path(home) / ".config";
  }
#endif
  throw std::runtime_error("Failed to determine app data directory");
}

void ensureDirectoriesExist() {
  fs::path basePath = getAppDataPath() / "tmplt";
  fs::path sourcesPath = basePath / "sources";
  fs::path templatesPath = basePath / "templates";

  fs::create_directories(basePath);
  fs::create_directories(sourcesPath);
  fs::create_directories(templatesPath);
}

#include "TemplateEngine.h" // Assuming you have a header file for your TemplateEngine class
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;

void handleGenerate(const CLI::ParsedArgs &args) {
  std::string templateName = args.arguments[0];

  // Define the path to your templates directory and the template file
  auto templatePath =
      getAppDataPath() / "tmplt" / "templates" / (templateName + ".template");

  // Check if the template file exists
  if (!fs::exists(templatePath)) {
    std::cerr << "Template file not found: " << templatePath << std::endl;
    return;
  }

  // Read the contents of the template file into a buffer
  std::ifstream templateFile(templatePath);
  if (!templateFile.is_open()) {
    std::cerr << "Error opening template file: " << templatePath << std::endl;
    return;
  }

  std::stringstream buffer;
  buffer << templateFile.rdbuf();
  std::string templateBuffer = buffer.str();

  // Deserialize the template from the buffer
  tmplt::Template tmpl;
  try {
    tmpl = tmplt::Template::deserialize(templateBuffer);
  } catch (const std::exception &e) {
    std::cerr << "Error deserializing template: " << e.what() << std::endl;
    return;
  }

  // Initialize the template engine
  tmplt::TemplateEngine engine;

  // If the "interactive" flag is present, we will handle it later
  if (args.options.count("interactive") > 0) {
    engine.interactiveGenerateTemplate(tmpl, fs::current_path());
    return;
  }

  // Iterate over arguments and set variable values for the template
  for (int i = 1; i < args.arguments.size(); i++) {
    std::string arg = args.arguments[i];
    size_t pos = arg.find("=");

    if (pos == std::string::npos)
      continue;

    std::string name = arg.substr(0, pos);
    std::string value = arg.substr(pos + 1);

    // Check if the variable exists in the template and set its value
    if (tmpl.variables.contains(name)) {
      std::cout << "Set '" << name << "' to '" << value << "'" << std::endl;
      tmpl.variables[name] =
          value; // Assuming you have a map or method to set variable values
    }
  }

  // You can now proceed with generating files using the modified `tmpl`
  // engine.generateFiles(tmpl, targetPath, ...);  // Proceed with generating
  // files
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

  auto tmpltFolder = getAppDataPath() / "tmplt";

  // Create template file
  auto filePath = tmpltFolder / "templates" / (tmpl->name + ".template");
  std::ofstream output(filePath);
  if (output) {
    output << tmpl->serialize();
    output.close();
    std::cout << tmplt::GREEN << "Template created successfully at '"
              << filePath << "'." << tmplt::RESET << std::endl;
  } else {
    throw std::runtime_error("Failed to create file at '" + filePath.string() +
                             "'.");
  }
}

int main(int argc, char *argv[]) {
  // Register command
  try {
    ensureDirectoriesExist();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
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
