using System.Text.Json;
using System.Text.Json.Serialization;
using Tmplt.Cli;
using Tmplt.Core;
using Tmplt.Core.Abstractions;
using Tmplt.Core.Converters;

IConsoleReader consoleReader = new ConsoleReader();
IInputProvider inputProvider = new CliInputProvider(consoleReader);
var templateConfigurator = new TemplateConfigurator(inputProvider);
var templateManager = new TemplateManager();

string templateDirectory = "/home/thiagomv/test/";
Template template = templateConfigurator.ConfigureTemplateFromDirectory(templateDirectory);

Console.ReadKey();
Console.Clear();
templateConfigurator.CreateProjectFromTemplate("/home/thiagomv/result", template);