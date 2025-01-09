namespace Tmplt.Core;

public record TemplateVariable
{
    public VariableType Type { get; set; } = VariableType.Undefined;
    public string Name { get; set; } = string.Empty;
    
    public string[] Values { get; set; } = [];

    public TemplateVariable()
    {
        
    }
    private TemplateVariable(VariableType type, string name)
    {
        Type = type;
        Name = name;
    }

    public static TemplateVariable Create(string name) => new(VariableType.Undefined, name);
}