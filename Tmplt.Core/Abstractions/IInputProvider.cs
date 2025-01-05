namespace Tmplt.Core.Abstractions;

public interface IInputProvider
{
    /// <summary>
    /// Displays a multiple-choice prompt and returns the selected option.
    /// </summary>
    T AskForChoice<T>(string prompt, Dictionary<T, string> options) where T : notnull;

    /// <summary>
    /// Asks the user to enter a value for a variable, validating the input.
    /// </summary>
    string AskForInput(string prompt, bool isMultiline = false);

    /// <summary>
    /// Asks the user to confirm an action (Yes/No).
    /// </summary>
    bool AskForConfirmation(string prompt);

    /// <summary>
    /// Provides a dynamic way to configure a variable (e.g., Replace or Conditional).
    /// </summary>
    TemplateVariable ConfigureVariable(TemplateVariable variable);
}