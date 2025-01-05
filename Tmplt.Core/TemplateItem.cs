using System.IO;
using System.Text.RegularExpressions;

namespace Tmplt.Core;

    public partial class TemplateItem
    {
        public string Path { get; set; }
        public bool IsFolder { get; set; }
        public string Content { get; set; }
        public List<TemplateVariable> Variables { get; set; } = new();

        public void ParseVariables()
        {
            var regex = VariableRegex();
            foreach (Match match in regex.Matches(Content))
            {
                if (match.Success)
                {
                    // Capture the inner part of the ${{ ... }}$ expression
                    var variableName = match.Groups[1].Value; // This is the text between ${{ and }}$
                    Variables.Add(TemplateVariable.Create(variableName));
                }
            }
        }

        public TemplateItem(string path)
        {
            Path = path;
            IsFolder = path.EndsWith('/') || path.EndsWith('\\');
            Content = "";
        }

        public TemplateItem(string path, string content)
        {
            IsFolder = path.EndsWith('/') || path.EndsWith('\\');
            Path = path;
            Content = content;
        }

        public TemplateItem(string path, string content, bool isFolder)
        {
            IsFolder = isFolder;
            Path = path;
            Content = content;
        }

        // Updated regex pattern with proper boundaries
        [GeneratedRegex(@"\$\{\{([^${}]+)\}\}\$", RegexOptions.Singleline)]
        private static partial Regex VariableRegex();
    }