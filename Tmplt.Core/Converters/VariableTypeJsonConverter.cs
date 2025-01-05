using System.Text.Json;
using System.Text.Json.Serialization;

namespace Tmplt.Core.Converters;

public class VariableTypeJsonConverter : JsonConverter<VariableType>
{
    public override VariableType Read(ref Utf8JsonReader reader, Type typeToConvert, JsonSerializerOptions options)
    {
        var value = reader.GetString();
        if (string.IsNullOrEmpty(value))
            throw new ArgumentNullException(nameof(value), "Value cannot be null or empty.");

        var resources = value.Split(", ", StringSplitOptions.RemoveEmptyEntries);

        if (Enum.TryParse(typeof(VariableType), value, true, out var parsedType))
        {
            return (VariableType)parsedType;
        }
        else
        {
            throw new JsonException($"Invalid VariableType value: {value}");
        }
    }

    public override void Write(Utf8JsonWriter writer, VariableType value, JsonSerializerOptions options)
    {
        writer.WriteStringValue(value.ToString());
    }
}