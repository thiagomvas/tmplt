using Cocona;
using Tmplt.Core;

namespace Tmplt.Cli.Commands;

public class ConfigureCommands
{
    private readonly TemplateConfigurator _templateConfigurator;
    
    public ConfigureCommands(TemplateConfigurator templateConfigurator)
    {
        _templateConfigurator = templateConfigurator;
    }
    public void New(
        [Argument(Order = 0, Description = "The name of the template")] string name, 
        [Argument(Order = 1, Description = "The root path used to begin configuring the template.")] string root,
        [Option(Description = "The description of the template.")] string description = "")
    {
        Console.WriteLine($"Configuring {root}");
        var res = _templateConfigurator.ConfigureTemplateFromDirectory(root);
        res.Name = name;
        res.Description = description;
        var json = res.Serialize();
        
        var resultPath = Path.Combine(Constants.AppDataPath, $"{name.ToLowerInvariant()}.json");
        File.WriteAllText(resultPath, json);
    }
    
    public void Delete(
        [Argument(Order = 0, Description = "The name of the template")] string name)
    {
        var path = Path.Combine(Constants.AppDataPath, $"{name.ToLowerInvariant()}.json");
        if (File.Exists(path))
        {
            File.Delete(path);
        }
    }

    public void List()
    {
        var path = Constants.AppDataPath;
        
        var templates = Directory.GetFiles(path, "*.json")
            .Select(f => File.ReadAllText(f))
            .Select(j => Template.Deserialize(j));
        foreach (var template in templates)
        {
            Console.WriteLine($"{template.Name} - {template.Description}");
        }
    }
}