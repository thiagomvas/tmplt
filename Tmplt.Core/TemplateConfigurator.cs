using Tmplt.Core.Abstractions;

namespace Tmplt.Core;

public class TemplateConfigurator
{
    private readonly IInputProvider _inputProvider;

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

    public Template ConfigureTemplateFromDirectory(string directory)
    {
        var template = new TemplateManager().FromDirectory(directory);

        foreach (var item in template.Items)
        {
            item.ParseVariables();
            foreach (var variable in item.Variables)
            {
                if (variable.Type == VariableType.Undefined)
                {
                    // Use IInputProvider to configure the variable
                    _inputProvider.ConfigureVariable(variable);
                }
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

        foreach (var item in template.Items)
        {
            string itemPath = Path.Combine(targetPath, item.Path);

            if (item.IsFolder)
            {
                Directory.CreateDirectory(itemPath);
                Console.WriteLine($"Created directory: {item.Path}");
            }
            else
            {
                string content = ProcessItemContent(item);
                File.WriteAllText(itemPath, content);
                Console.WriteLine($"Created file: {item.Path}");
            }
        }
    }

    /// <summary>
    /// Processes the item content, replacing variables with user input.
    /// </summary>
    private string ProcessItemContent(TemplateItem item)
    {
        string resultContent = item.Content;
        foreach (var variable in item.Variables)
        {
            if (variable.Type == VariableType.Replace)
            {
                // Ask for user input for the replacement
                string replacement = _inputProvider.AskForInput(variable.Name);
                resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
            }
            else if (variable.Type == VariableType.Conditional)
            {
                // Ask for user input to decide on the conditional value
                bool isTrue = _inputProvider.AskForConfirmation(variable.Name);
                resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$",
                    isTrue ? variable.TrueValue : variable.FalseValue);
            }
        }

        return resultContent;
    }
}