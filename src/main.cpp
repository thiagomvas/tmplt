#include "cli.h"
#include <iostream>

int main(int argc, char *argv[]) {
  // Register commands
  CLI::registerCommand("copy", "Copies files from source to destination.");
  CLI::registerCommand("delete", "Deletes specified files.");

  // Register options per command
  CLI::registerOption("copy", "recursive", CLI::OptionType::Boolean,
                      "Copy directories recursively", "r");
  CLI::registerOption("copy", "destination", CLI::OptionType::String,
                      "Specify destination directory", "d");

  // Register arguments
  CLI::registerArgument("copy", "source", "Source file(s) to copy");
  CLI::registerArgument("copy", "destination", "Destination directory");

  try {
    auto args = CLI::parse(argc, argv);
    std::cout << "Command: " << args.command << "\n";

    for (const auto &[key, value] : args.options) {
      std::cout << "Option --" << key << " = " << value << "\n";
    }

    for (const auto &arg : args.arguments) {
      std::cout << "Argument: " << arg << "\n";
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
