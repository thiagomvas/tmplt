using System.Text.Json;
using System.Text.Json.Serialization;
using Tmplt.Core.Converters;

namespace Tmplt.Core;

public class Template
{
    public string Name { get; set; }
    public string Description { get; set; }
    public List<TemplateItem> Items { get; set; } = new();
    
    public string Serialize()
    {
        return JsonSerializer.Serialize(this, new JsonSerializerOptions
        {
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
            WriteIndented = true,
            Converters =
            {
                new VariableTypeJsonConverter()
            }
        });
    }

    public static Template? Deserialize(string template)
    {
        return JsonSerializer.Deserialize<Template>(template, new JsonSerializerOptions()
        {
            Converters =
            {
                new VariableTypeJsonConverter()
            }
        });
    }
}