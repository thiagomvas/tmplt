
#include "cli.h"
#include <iostream>

const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";
const std::string MAGENTA = "\033[35m";

CLI::CLI(tmplt::TemplateEngine &engine) : engine(engine) {}

void CLI::printUsage() const {
  std::cout
      << CYAN << "Usage:\n"
      << RESET
      << "  tmpl create <file>              Create a template from a file\n"
      << "  tmpl apply <template> <output>  Apply a template\n"
      << "  tmpl list                       List stored templates\n"
      << "  tmpl --help                     Show this help message\n";
}

void CLI::run(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << RED << "Error: No command provided.\n" << RESET;
    printUsage();
    return;
  }

  std::string command = argv[1];
  std::vector<std::string> args(argv + 2, argv + argc);

  if (command == "create") {
    handleCreate(args);
  } else if (command == "apply") {
    handleApply(args);
  } else if (command == "list") {
    handleList();
  } else if (command == "--help" || command == "-h") {
    printUsage();
  } else {
    std::cerr << RED << "Error: Unknown command '" << command << "'.\n"
              << RESET;
    printUsage();
  }
}

void CLI::handleCreate(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cerr << RED << "Error: No file provided.\n" << RESET;
    return;
  }

  std::string name, description;

  std::cout << CYAN << "Enter template name: " << RESET;
  std::getline(std::cin, name);

  std::cout << CYAN << "Enter template description: " << RESET;
  std::getline(std::cin, description);

  auto tmpl = engine.createSingleFileTemplate(args[0]);
  tmpl.name = name;
  tmpl.description = description;

  for (auto &pair : tmpl.variables) {
    configureVariable(pair.second);
  }

  std::cout << GREEN << "Template created successfully:\n" << RESET;
  std::cout << tmpl.serialize() << std::endl;
}

void CLI::handleApply(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    std::cerr << RED << "Error: Missing arguments for apply.\n" << RESET;
    return;
  }

  // Placeholder for applying a template
  std::cout << YELLOW << "Applying template " << args[0] << " to " << args[1]
            << "\n"
            << RESET;
}

void CLI::handleList() {
  std::cout << MAGENTA << "Listing available templates...\n" << RESET;
}

void CLI::configureVariable(tmplt::TemplateVariable &variable) {
  std::cout << CYAN << "Configuring variable: " << variable.name << RESET
            << "\n";

  // Modify description
  std::string newDescription;
  std::cout << CYAN << "Current description: " << variable.description << "\n";
  std::cout << CYAN
            << "Enter new description (leave empty to keep current): " << RESET;
  std::getline(std::cin, newDescription);
  if (!newDescription.empty()) {
    variable.description = newDescription;
  }

  // Modify type
  std::cout << CYAN << "Current type: " << static_cast<int>(variable.type)
            << "\n";
  std::cout << CYAN << "Enter new type (1: Text, 2: Bool, 3: Enum): " << RESET;
  int newType;
  std::cin >> newType;
  std::cin.ignore(); // to discard the newline character

  if (newType > 0 && newType <= 3) {
    variable.type = static_cast<tmplt::VariableType>(newType);
  } else {
    std::cout << RED << "Invalid type. Keeping current type." << RESET << "\n";
  }
}
