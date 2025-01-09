# Tmplt - A way to generate projects following your own custom templates.
Tmplt is a CLI tool designed to create templates based on a folder's structure, with support for configurable variables.

## Features
- **Folder Structure Replication**: Tmplt can replicate the structure of a folder, including files and subfolders.
- **Custom Variables**: Tmplt allows you to define variables in your templates, which will be prompted to the user when generating a project.
- **Cross-Platform**: Tmplt is designed to work on Windows, Linux, and MacOS.
- **Quick and Simple CLI**: Tmplt is designed to be simple and easy to use, with a minimalistic CLI.
## Usage
### Creating a template
1. Create a folder with the desired structures for your templates, including any files you wish to include.
2. Add placeholders for variables following the format `${{variable_name}}$`.
3. Run the following command to save the folder as template
```bash
tmplt config new <template-name> <path-to-folder>
```

### Generating a project from a template
1. Run the following command to generate a project from a template
```bash
tmplt create <template-name> <path-to-output>
```
> [!IMPORTANT]
> Tmplt will create the files **in that directory** rather than creating a new directory with the template's name. Files can and will be overwritten if they already exist.

The command will prompt you to fill in the variables you defined in the template. After filling in the variables, the project will be generated.

### Modifying an existing template
You can either regenerate the template with the same name, or manually go to the folder where templates are configured

The templates are located in the following directory:
#### Windows
```bash
%APPDATA%\tmplt\
```

#### Linux
```bash
~/.config/tmplt/
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
This project is licensed under the [MIT License](https://choosealicense.com/licenses/mit/)

