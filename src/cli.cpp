
#include "cli.h"
#include <iostream>

std::unordered_map<std::string, CLI::Command> CLI::registeredCommands;

void CLI::registerCommand(const std::string &name,
                          const std::string &description) {
  registeredCommands[name] = Command{description, {}, {}};
}

void CLI::registerOption(const std::string &command,
                         const std::string &longName, OptionType type,
                         const std::string &description,
                         const std::string &shortName) {
  if (registeredCommands.count(command) == 0) {
    throw std::runtime_error("Cannot register option for unknown command: " +
                             command);
  }
  registeredCommands[command].options[longName] = {type, description,
                                                   shortName};
}

void CLI::registerArgument(const std::string &command, const std::string &name,
                           const std::string &description) {
  if (registeredCommands.count(command) == 0) {
    throw std::runtime_error("Cannot register argument for unknown command: " +
                             command);
  }
  registeredCommands[command].arguments.push_back({name, description});
}

CLI::ParsedArgs CLI::parse(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("No command provided.");
  }

  std::string command = argv[1];

  // Handle global help request (e.g., tmplt -h or tmplt --help)
  if (command == "-h" || command == "--help") {
    std::cout << "Available commands:\n";
    for (const auto &[cmd, info] : registeredCommands) {
      std::cout << "  " << cmd << " - " << info.description << "\n";
    }
    exit(0);
  }

  // Validate command
  if (registeredCommands.find(command) == registeredCommands.end()) {
    throw std::runtime_error("Unknown command: " + command);
  }

  const Command &cmd = registeredCommands[command];

  // Check if help is requested for a specific command
  if (argc > 2 &&
      (std::string(argv[2]) == "-h" || std::string(argv[2]) == "--help")) {
    std::cout << "Usage: tmplt " << command << " [OPTIONS] [ARGUMENTS]\n\n";
    std::cout << cmd.description << "\n\n";

    if (!cmd.options.empty()) {
      std::cout << "Options:\n";
      for (const auto &[longOpt, opt] : cmd.options) {
        std::cout << "  --" << longOpt;
        if (!opt.shortForm.empty()) {
          std::cout << ", -" << opt.shortForm;
        }
        std::cout << " ("
                  << (opt.type == OptionType::String ? "string" : "flag")
                  << ") - " << opt.description << "\n";
      }
      std::cout << "\n";
    }

    if (!cmd.arguments.empty()) {
      std::cout << "Arguments:\n";
      for (const auto &arg : cmd.arguments) {
        std::cout << "  " << arg.name << " - " << arg.description << "\n";
      }
      std::cout << "\n";
    }

    exit(0);
  }

  ParsedArgs result;
  result.command = command;

  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg.rfind("--", 0) == 0) { // Long option
      std::string key = arg.substr(2);

      if (cmd.options.count(key) == 0) {
        throw std::runtime_error("Unknown option: --" + key);
      }

      if (cmd.options.at(key).type == OptionType::String) {
        if (i + 1 < argc) {
          result.options[key] =
              argv[++i]; // Take the next argument as the value
        } else {
          throw std::runtime_error("Option --" + key + " requires a value.");
        }
      } else {
        result.options[key] = "true"; // Boolean flag
      }

    } else if (arg.rfind("-", 0) == 0 && arg.size() > 1) { // Short option
      std::string shortKey = arg.substr(1);
      bool found = false;

      for (const auto &[longOpt, option] : cmd.options) {
        if (option.shortForm == shortKey) {
          found = true;
          if (option.type == OptionType::String) {
            if (i + 1 < argc) {
              result.options[longOpt] = argv[++i];
            } else {
              throw std::runtime_error("Option -" + shortKey +
                                       " requires a value.");
            }
          } else {
            result.options[longOpt] = "true";
          }
          break;
        }
      }

      if (!found) {
        throw std::runtime_error("Unknown option: -" + shortKey);
      }

    } else { // Positional argument
      result.arguments.push_back(arg);
    }
  }

  return result;
}
