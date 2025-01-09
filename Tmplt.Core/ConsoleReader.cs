using Tmplt.Core.Abstractions;

namespace Tmplt.Core;

public class ConsoleReader : IConsoleReader
{
    public string ReadLine() => Console.ReadLine()!;

    public string ReadMultiline()
    {
        var lines = new List<string>();
        while (true)
        {
            var line = Console.ReadLine();
            if (string.IsNullOrWhiteSpace(line))
                break;
            lines.Add(line);
        }
        return string.Join(Environment.NewLine, lines);
    }
}