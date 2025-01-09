using Cocona;
using Microsoft.Extensions.DependencyInjection;
using Tmplt.Cli;
using Tmplt.Cli.Commands;
using Tmplt.Core;
using Tmplt.Core.Abstractions;

// Ensure config folder exists in application path
var appDataPath = Constants.AppDataPath;
if (!Directory.Exists(appDataPath))
    Directory.CreateDirectory(appDataPath);


var builder = CoconaApp.CreateBuilder();
builder.Services.AddScoped<IInputProvider, CliInputProvider>();
builder.Services.AddScoped<IConsoleReader, ConsoleReader>();
builder.Services.AddScoped<TemplateConfigurator>();

var app = builder.Build();

app.AddSubCommand("config", x => x.AddCommands<ConfigureCommands>());
app.AddSubCommand("create", x => x.AddCommands<CreateCommands>());

app.Run();
