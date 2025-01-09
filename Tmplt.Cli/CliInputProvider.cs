using Tmplt.Core;
using Tmplt.Core.Abstractions;
namespace Tmplt.Cli;
public class CliInputProvider : IInputProvider
{
    private readonly IConsoleReader _consoleReader;
    private HashSet<string> _configuredVariables = new();

    public CliInputProvider(IConsoleReader consoleReader)
    {
        _consoleReader = consoleReader;
    }

    public T AskForEnum<T>(string prompt) where T : Enum
    {
        var values = Enum.GetValues(typeof(T)).Cast<T>().ToList();
        var options = values.ToDictionary(v => v, v => v.ToString());
        return AskForChoice(prompt, options);
    }
    /// <summary>
    /// Displays a multiple-choice prompt and returns the selected option.
    /// </summary>
    public T AskForChoice<T>(string prompt, Dictionary<T, string> options) where T : notnull
    {
        // Displaying prompt in bold-like format
        Console.ForegroundColor = ConsoleColor.Cyan;
        Console.WriteLine($"--- {prompt} ---");
        Console.ResetColor();
        
        // Adding spacing between prompt and options for clarity
        Console.WriteLine();
        
        int index = 1;
        foreach (var option in options)
        {
            // Option number in bold to make it stand out
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"{index++}. {option.Value}");
            Console.ResetColor();
        }

        T selectedValue;
        while (true)
        {
            var input = _consoleReader.ReadLine();
            if (int.TryParse(input, out int choice) && choice >= 1 && choice <= options.Count)
            {
                selectedValue = options.Keys.ElementAt(choice - 1);
                break;
            }
            else
            {
                // Invalid input with a clear warning in red
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Invalid choice. Please select a valid option.");
                Console.ResetColor();
                Console.WriteLine(); // Adding a blank line for better readability
            }
        }

        return selectedValue;
    }

    /// <summary>
    /// Asks the user to enter a value for a variable, validating the input.
    /// </summary>
    public string AskForInput(string prompt, bool isMultiline = false)
    {
        // Prompt displayed in blue
        Console.ForegroundColor = ConsoleColor.Blue;
        if(!isMultiline)
            Console.Write($"{prompt}: ");
        else 
            Console.WriteLine($"{prompt}: ");
        Console.ResetColor();
        return isMultiline ? _consoleReader.ReadMultiline() : _consoleReader.ReadLine();
    }

    /// <summary>
    /// Asks the user to confirm an action (Yes/No).
    /// </summary>
    public bool AskForConfirmation(string prompt)
    {
        // Enhanced visual prompt in yellow
        Console.ForegroundColor = ConsoleColor.Yellow;
        Console.Write($"{prompt} (Y/N): ");
        Console.ResetColor();

        while (true)
        {
            var input = _consoleReader.ReadLine().Trim().ToLower();
            if (input == "y" || input == "yes")
                return true;
            else if (input == "n" || input == "no")
                return false;

            // Invalid input with error message in red
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Invalid input. Please enter 'Y' for Yes or 'N' for No.");
            Console.ResetColor();
            Console.WriteLine(); // Adding a blank line for better readability
        }
    }

    /// <summary>
    /// Provides a dynamic way to configure a variable (e.g., Replace or Conditional).
    /// </summary>
    public TemplateVariable ConfigureVariable(TemplateVariable variable)
    {
        if (_configuredVariables.Contains(variable.Name))
        {
            return variable;
        }
        var type = AskForEnum<VariableType>($"Select a type for the variable '{variable.Name}'");

        variable.Type = type;

        if (type == VariableType.Conditional)
        {
            // Enhanced section header for better visibility
            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine($"--- Configure the conditional replacement for '{variable.Name}': ---");
            Console.ResetColor();
            variable.Values = new string[2];
            Console.WriteLine($"If true, replace with (Enter on empty line ends reading): ");
            variable.Values[0] = _consoleReader.ReadMultiline();
            Console.WriteLine($"If false, replace with (Enter on empty line ends reading): ");
            variable.Values[1] = _consoleReader.ReadMultiline();
        }
        else if (type == VariableType.Enum)
        {
            var options = AskForInput("Comma-separated list of Enum options");
            var optionArr = options.Split(',').Select(o => o.Trim()).ToArray();
            var optionCount = optionArr.Length;
            variable.Values = new string[optionCount + 1];
            variable.Values[0] = options;
            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine($"--- Configure the Enum options replacement for '{variable.Name}': ---");
            for (int i = 1; i <= optionCount; i++)
            {
                variable.Values[i] = AskForInput(optionArr[i - 1]);
            }
        }

        _configuredVariables.Add(variable.Name);
        return variable;
    }
}
