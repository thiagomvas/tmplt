namespace Tmplt.Cli;

public static class Constants
{
    public static string AppDataPath => Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Tmplt");
    
}