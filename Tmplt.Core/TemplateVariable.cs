namespace Tmplt.Core;

public record TemplateVariable
{
    public VariableType Type { get; set; } = VariableType.Undefined;
    public string Name { get; set; } = string.Empty;
    public string? TrueValue { get; set; }
    public string? FalseValue { get; set; }

    private TemplateVariable(VariableType type, string name,
        string? trueValue = null, string? falseValue = null)
    {
        Type = type;
        Name = name;
        TrueValue = trueValue;
        FalseValue = falseValue;
    }

    public static TemplateVariable Create(string name) => new(VariableType.Undefined, name);

    public static TemplateVariable CreateConditional(string name, string trueValue, string falseValue) =>
        new(VariableType.Conditional, name, trueValue, falseValue);

    public static TemplateVariable CreateReplace(string name, string replaceValue) =>
        new(VariableType.Replace, name);
}