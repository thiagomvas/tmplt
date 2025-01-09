using Tmplt.Core.Abstractions;

namespace Tmplt.Core;

/// <summary>
/// Responsible for configuring templates and creating a project from them.
/// </summary>
public class TemplateConfigurator
{
    private readonly IInputProvider _inputProvider;
    private readonly Dictionary<string, (string selectedValue, string replacement)> _preConfiguredVariables = new();

    public TemplateConfigurator(IInputProvider inputProvider)
    {
        _inputProvider = inputProvider;
    }

    /// <summary>
    /// Configures a template from a directory and creates a project dynamically.
    /// </summary>
    public void CreateProjectFromTemplate(string targetPath, string templateDirectory)
    {
        var template = ConfigureTemplateFromDirectory(templateDirectory);

        // Create the directories and files from the configured template
        CreateFromTemplateDynamic(targetPath, template);
    }

    /// <summary>
    /// Creates a project dynamically from an already configured template.
    /// </summary>
    public void CreateProjectFromTemplate(string targetPath, Template template) =>
        CreateFromTemplateDynamic(targetPath, template);

    /// <summary>
    /// Creates a template from a directory and returns it.
    /// </summary>
    /// <param name="directory">The root directory to configure a template from.</param>
    /// <returns>An unnamed template from a directory.</returns>
    public Template ConfigureTemplateFromDirectory(string directory)
    {
        var template = new TemplateManager().FromDirectory("unnamed", directory);
        template.ParseVariables();

        foreach (var variable in template.Variables)
        {
            if (variable.Type == VariableType.Undefined)
            {
                // Use IInputProvider to configure the variable
                _inputProvider.ConfigureVariable(variable);
            }
        }
        
        // Configure item variable dependencies
        foreach (var item in template.Items)
        {
            bool hasVariableDependency = _inputProvider.AskForConfirmation($"Does {item.Path} depend on a variable?");
            if (hasVariableDependency)
            {
                var variableOptionsDict = template.Variables.Select((v, i) => new { Key = i + 1, Value = v.Name })
                    .ToDictionary(x => x.Key, x => x.Value);
                var variableIndex = _inputProvider.AskForChoice("Select the variable that this item depends on", variableOptionsDict);
                item.DependsOnVariable = variableOptionsDict[variableIndex];
                item.VariableValue = _inputProvider.AskForInput("What value should the variable have?");
                item.InvertVariable = _inputProvider.AskForConfirmation("Should the variable value be inverted?(E.g., create if variable is NOT this value)");
            }
        }

        return template;
    }


    /// <summary>
    /// Creates the files and directories dynamically from the template.
    /// </summary>
    private void CreateFromTemplateDynamic(string targetPath, Template template)
    {
        if (!Directory.Exists(targetPath))
            Directory.CreateDirectory(targetPath);
        
        // Preconfigure the variables with user input before replacing to make sure that all variables are replaced, including on file path.
        PreconfigureVariableReplacements(template);

        foreach (var item in template.Items)
        {
            // If the item has a variable it depends on, check if it should be created
            if (!string.IsNullOrWhiteSpace(item.DependsOnVariable))
            {
                // If inverted, check if the value is anything but the variable value, if it is the same value, skip this item.
                if (item.InvertVariable)
                {
                    if(_preConfiguredVariables.TryGetValue(item.DependsOnVariable, out var value) 
                       && string.Equals(value.selectedValue, item.VariableValue, StringComparison.OrdinalIgnoreCase))
                        continue;
                }
                else // Otherwise, check if it is the same value. If it isn't, skip this item.
                {
                    if(!_preConfiguredVariables.TryGetValue(item.DependsOnVariable, out var value) 
                       || !string.Equals(value.selectedValue, item.VariableValue, StringComparison.OrdinalIgnoreCase))
                        continue;
                }
            }
            string itemPath = Path.Combine(targetPath, item.Path);
            foreach(var variable in template.Variables)
            {
                if (_preConfiguredVariables.TryGetValue(variable.Name, out var value))
                {
                    itemPath = itemPath.Replace($"${{{{{variable.Name}}}}}$", value.replacement);
                }
            }

            if (item.IsFolder)
            {
                Directory.CreateDirectory(itemPath);
                Console.WriteLine($"Created directory: {itemPath}");
            }
            else
            {
                string content = ProcessItemContent(template, item);
                File.WriteAllText(itemPath, content);
                Console.WriteLine($"Created file: {itemPath}");
            }
        }
    }

    /// <summary>
    /// Processes the item content, replacing variables with user input.
    /// </summary>
    private string ProcessItemContent(Template template, TemplateItem item)
    {
        string resultContent = item.Content;
        foreach (var variable in template.Variables)
        {
            if (_preConfiguredVariables.TryGetValue(variable.Name, out var value))
            {
                resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", value.replacement);
                continue;
            }
        }

        return resultContent;
    }

    private void PreconfigureVariableReplacements(Template template)
    {
        foreach (var item in template.Items)
        {
            string resultContent = item.Content;
            foreach (var variable in template.Variables)
            {
                string replacement = string.Empty;
                string selectedValue = string.Empty;
                if (_preConfiguredVariables.TryGetValue(variable.Name, out var value))
                {
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", value.replacement);
                    continue;
                }

                if (variable.Type == VariableType.Conditional)
                {
                    // Ask for user input to decide on the conditional value
                    bool isTrue = _inputProvider.AskForConfirmation(variable.Name);
                    replacement = isTrue ? variable.Values[0] : variable.Values[1];
                    selectedValue = isTrue.ToString();
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
                }
                else if (variable.Type == VariableType.SingleLine)
                {
                    var response = _inputProvider.AskForInput(variable.Name);
                    replacement = response;
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
                }
                else if (variable.Type == VariableType.Multiline)
                {
                    var response = _inputProvider.AskForInput(variable.Name, true);
                    replacement = response;
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
                }
                else if (variable.Type == VariableType.Enum)
                {
                    var options = variable.Values[0].Split(',').Select(o => o.Trim()).ToArray();
                    var optionsDict = options.Select((o, i) => new { Key = i + 1, Value = o })
                        .ToDictionary(x => x.Key, x => x.Value);

                    var response = _inputProvider.AskForChoice(variable.Name, optionsDict);
                    replacement = variable.Values[response];
                    selectedValue = optionsDict[response];
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
                }

                if (!string.IsNullOrWhiteSpace(replacement))
                    _preConfiguredVariables[variable.Name] = (selectedValue, replacement);
            }
        }
    }
}