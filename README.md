# tmplt - File Template Engine CLI

**tmplt** is a command-line tool that helps you create, manage, and apply customizable file templates. It allows you to define templates for files or directories with placeholders and logic, making it easy to automate the creation of standardized files in any project.

With **tmplt**, you can define flexible templates with variables, conditional file inclusion, and an interactive tool to configure new templates. It's designed to be language and IDE agnostic, offering a powerful way to automate file generation in any development environment.

## Features

- **Create Custom Templates**: Define reusable templates for files or directories with dynamic placeholders.
- **Variable Support**: Easily replace variables in your templates with dynamic values when generating files.
- **Conditional File Inclusion**: Include or exclude files based on the value of a variable or condition.
- **Interactive Template Configuration**: An interactive tool that helps you configure new templates without manually editing text files.
- **Raw Text Format**: Templates are stored in a raw text format, making them simple to edit, version, and share.

## Installation

To install **tmplt**, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/tmplt.git
   cd tmplt
   ```

2. Build the project:
   ```bash
   make
   ```

3. Optionally, install the CLI tool globally:
   ```bash
   sudo make install
   ```

## Usage

### Create a Template

To create a new template, use the following command:
```bash
tmplt create <template-name>
```
This command will walk you through the process of setting up your template, allowing you to define variables, file structure, and logic for conditional file inclusion.

## Contributing

Contributions are welcome! If you find a bug or have a feature suggestion, feel free to open an issue or submit a pull request.
