using System.Drawing;

namespace Tmplt.Core;

public static class TemplateEngine
{
    public static void CreateFromTemplate(string targetPath, Template template)
    {
        if(!Directory.Exists(targetPath))
            Directory.CreateDirectory(targetPath);

        foreach (var item in template.Items)
        {
            if (item.IsFolder)
            {
                Directory.CreateDirectory(Path.Combine(targetPath, item.Path));
                Console.WriteLine($"Created directory: {item.Path}");
            }
            else
            {
                File.WriteAllText(Path.Combine(targetPath, item.Path), item.Content);
                Console.WriteLine($"Created file: {item.Path}");
            }
        }
        
    }
    public static Template FromDirectory(string directory)
    {
        // Ensure the directory exists
        if (!Directory.Exists(directory))
        {
            throw new DirectoryNotFoundException($"The directory '{directory}' does not exist.");
        }

        var template = new Template
        {
            Name = new DirectoryInfo(directory).Name // Set the template's name to the root folder name
        };

        var items = new List<TemplateItem>();
        
        // Get all files in the directory and subdirectories
        var files = Directory.GetFiles(directory, "*.*", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            try
            {
                var content = File.ReadAllText(file); // Read file content
                var path = Path.GetRelativePath(directory, file);
                var item = new TemplateItem(path, content, false);
                items.Add(item);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to read file {file}: {ex.Message}");
            }
        }
        
        var directories = Directory.GetDirectories(directory, "*", SearchOption.AllDirectories);
        foreach (var dir in directories)
        {
            try
            {
                // Create a TemplateItem for each directory (without content)
                var path = Path.GetRelativePath(directory, dir);
                var item = new TemplateItem(path, "", true);
                items.Add(item);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to read directory {dir}: {ex.Message}");
            }
        }

        // Assign the collected items to the template
        template.Items = items.OrderByDescending(i => i.IsFolder).ToList();
        foreach(var item in template.Items)
            item.ParseVariables();
        return template;
    }

    public static void CreateFromTemplateDynamic(string targetPath, Template template)
    {
        foreach (var item in template.Items)
        {
            if (item.IsFolder)
            {
                Directory.CreateDirectory(Path.Combine(targetPath, item.Path));
                Console.WriteLine($"Created directory: {item.Path}");
            }
            else
            {
                CreateItemDynamic(targetPath, item);
                Console.WriteLine($"Created file: {item.Path}");
            }
        }
    }

    public static Template ConfigureTemplateFromDirectory(string directory)
    {
        var template = FromDirectory(directory);

        foreach (var item in template.Items)
        {
            foreach (var variable in item.Variables)
            {
                if (variable.Type == VariableType.Undefined)
                {
                    DynamicConfigureVariable(variable);
                }
            }
        }

        return template;
    }

    private static void DynamicConfigureVariable(TemplateVariable variable)
    {
        var type = RequestType(variable.Name);
        switch (type)
        {
            case VariableType.Replace:
                variable.Type = VariableType.Replace;
                break;
            case VariableType.Conditional:
                variable.Type = VariableType.Conditional;
                Console.WriteLine($"Replace '{variable.Name}' if true with (Enter on empty line ends reading): ");
                string trueValue = ReadMultiline();
                Console.WriteLine($"Replace '{variable.Name}' if false with (Enter on empty line ends reading): ");
                string falseValue = ReadMultiline();
                variable.TrueValue = trueValue;
                variable.FalseValue = falseValue;
                break;
            default:
                break;
        }
    }

    private static string ReadMultiline()
    {
        List<string> lines = new List<string>();
        while (true)
        {
            var line = Console.ReadLine();
            if (string.IsNullOrWhiteSpace(line))
                break;
            lines.Add(line);
        }
        return string.Join(Environment.NewLine, lines);
    }
    private static VariableType RequestType(string name)
    {
        while (true)
        {
            Console.WriteLine($"Please select a type for variable {name}:");
            Console.WriteLine("(C)onditional, (R)eplace");
            
            var input = Console.ReadLine().Trim().ToLowerInvariant();

            if (input is "conditional" or "c")
                return VariableType.Conditional;
            else if (input is "replace" or "r")
                return VariableType.Replace;
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Invalid input, please try again.");
            Console.ResetColor();
        }

        return VariableType.Undefined;
    }

    private static void CreateItemDynamic(string targetPath, TemplateItem item)
    {
        string resultContent = item.Content;
        foreach (var variable in item.Variables)
        {
            Console.WriteLine($"{variable.Name}: ");
            switch (variable.Type)
            {
                case VariableType.Replace:
                    string replacement = Console.ReadLine();
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", replacement);
                    break;
                case VariableType.Conditional:
                    Console.Write("[(T)rue, (F)alse]: ");
                    string value = Console.ReadLine();
                    bool isTrue = value.Trim().ToLowerInvariant() switch
                    {
                        "true" or "t" => true,
                        "false" or "f" => false,
                        _ => false
                    };
                    resultContent = resultContent.Replace($"${{{{{variable.Name}}}}}$", isTrue ? variable.TrueValue : variable.FalseValue);
                    break;
                default:
                    break;
            }
        }
        File.WriteAllText(Path.Combine(targetPath, item.Path), resultContent);
    }
}