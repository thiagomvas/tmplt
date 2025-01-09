using Cocona;
using Tmplt.Core;

namespace Tmplt.Cli.Commands;

public class CreateCommands
{
    private readonly TemplateConfigurator _templateConfigurator;
    
    public CreateCommands(TemplateConfigurator templateConfigurator)
    {
        _templateConfigurator = templateConfigurator;
    }
    
    public void Create(
        [Argument(Order = 0, Description = "The name of the template")] string name, 
        [Argument(Order = 1, Description = "The root path used to begin creating on. NOTE: It will copy the exact structure into root, overriding any existing files.")] string root)
    {
        var path = Path.Combine(Constants.AppDataPath, $"{name}.json");
        if (!File.Exists(path))
        {
            Console.WriteLine($"Template '{name}' does not exist. Use 'tmplt config new {name} <root-path>' to create it.");
            return;
        }
        
        var template = Template.Deserialize(File.ReadAllText(path));
        
        _templateConfigurator.CreateProjectFromTemplate(root, template);
    }
}