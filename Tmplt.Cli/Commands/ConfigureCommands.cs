using System.ComponentModel;
using Cocona;
using SharpTables;
using Tmplt.Core;

namespace Tmplt.Cli.Commands;

public class ConfigureCommands
{
    private readonly TemplateConfigurator _templateConfigurator;
    
    public ConfigureCommands(TemplateConfigurator templateConfigurator)
    {
        _templateConfigurator = templateConfigurator;
    }
    [Command(Description = "Create a template based off of a directory's structure.")]
    public void New(
        [Argument(Order = 0, Description = "The name of the template")] string name, 
        [Argument(Order = 1, Description = "The root path used to begin configuring the template.")] string root,
        [Option('d' ,Description = "The description of the template.")] string description = "",
        [Option('t', Description = "A comma-separated list of tags for the template.")] string tags = "")
    {
        Console.WriteLine($"Configuring {root}");
        var res = _templateConfigurator.ConfigureTemplateFromDirectory(root);
        res.Name = name;
        res.Description = description;
        res.Tags = tags.Split(',').Select(t => t.Trim()).ToList();
        var json = res.Serialize();
        
        var resultPath = Path.Combine(Constants.AppDataPath, $"{name.ToLowerInvariant()}.json");
        File.WriteAllText(resultPath, json);

        var table = new Table();
        Console.WriteLine("Files in template: ");
        table.SetHeader(new Row("Path", "Depends on Variable", "Variable Value", "Invert Variable"));
        foreach (var item in res.Items)
        {
            table.AddRow(new Row(item.Path, item.DependsOnVariable, item.VariableValue, item.InvertVariable));
        }
        table.UseFormatting(TableFormatting.Minimalist)
            .Write();

        Console.WriteLine();
        
        Console.WriteLine("Variables in template: ");
        table = new Table();
        table.SetHeader(new Row("Name", "Type", "Options"));
        foreach (var variable in res.Variables)
        {
            string options = variable.Type switch
            {
                VariableType.Enum => variable.Values[0],
                VariableType.Conditional => "True / False",
                _ => "N/A"
            };
            table.AddRow(new Row(variable.Name, variable.Type, options));
        }
        table.UseFormatting(TableFormatting.Minimalist)
            .Write();
    }
    
    [Command(Description = "Delete a template.")]
    public void Delete(
        [Argument(Order = 0, Description = "The name of the template")] string name)
    {
        var path = Path.Combine(Constants.AppDataPath, $"{name.ToLowerInvariant()}.json");
        if (File.Exists(path))
        {
            File.Delete(path);
        }
    }

    [Command(Description = "List all templates.")]
    public void List()
    {
        var path = Constants.AppDataPath;
        
        var templates = Directory.GetFiles(path, "*.json")
            .Select(f => File.ReadAllText(f))
            .Select(j => Template.Deserialize(j));
        var table = new Table();
        foreach (var template in templates)
        {
            // Clamp description to 50 chars
            if (template.Description.Length > 50)
            {
                template.Description = template.Description.Substring(0, 50) + "...";
            }
            table.AddRow(new Row(template.Name, template.Description, string.Join(',', template.Tags)));
        }

        table.UseFormatting(TableFormatting.Minimalist)
            .SetHeader(new Row("Name", "Description", "Tags"))
            .Write();
    }
}