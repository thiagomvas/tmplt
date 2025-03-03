
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CLI {
public:
  enum class OptionType { Boolean, String };

  struct Option {
    OptionType type;
    std::string description;
    std::string shortForm;
  };

  struct Argument {
    std::string name;
    std::string description;
  };

  struct Command {
    std::string description;
    std::unordered_map<std::string, Option> options;
    std::vector<Argument> arguments;
  };

  struct ParsedArgs {
    std::string command;
    std::unordered_map<std::string, std::string> options;
    std::vector<std::string> arguments;
  };

  static void registerCommand(const std::string &name,
                              const std::string &description);
  static void registerOption(const std::string &command,
                             const std::string &longName, OptionType type,
                             const std::string &description,
                             const std::string &shortName = "");
  static void registerArgument(const std::string &command,
                               const std::string &name,
                               const std::string &description);
  static ParsedArgs parse(int argc, char *argv[]);

private:
  static std::unordered_map<std::string, Command> registeredCommands;
};
