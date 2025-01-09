using System.IO;
using System.Text.RegularExpressions;

namespace Tmplt.Core;

    public partial class TemplateItem
    {
        public string Path { get; set; }
        public bool IsFolder { get; set; }
        public string Content { get; set; }
        
        /// <summary>
        /// A variable that this item depends on.
        /// </summary>
        public string? DependsOnVariable { get; set; }
        
        /// <summary>
        /// The value of the variable that this item depends on. This file will only be created if the variable is set to this value
        /// </summary>
        public string? VariableValue { get; set; }

        public bool InvertVariable { get; set; } = false;
        


        internal List<TemplateVariable> ParseVariables()
        {
            var result = new List<TemplateVariable>();
            var regex = VariableRegex();
            foreach (Match match in regex.Matches(Path))
            {
                if (match.Success)
                {
                    // Capture the inner part of the ${{ ... }}$ expression
                    var variableName = match.Groups[1].Value; // This is the text between ${{ and }}$
                    result.Add(TemplateVariable.Create(variableName));
                }
            }
            foreach (Match match in regex.Matches(Content))
            {
                if (match.Success)
                {
                    // Capture the inner part of the ${{ ... }}$ expression
                    var variableName = match.Groups[1].Value; // This is the text between ${{ and }}$
                    result.Add(TemplateVariable.Create(variableName));
                }
            }

            return result;
        }

        public TemplateItem()
        {
            Path = "";
            IsFolder = false;
            Content = "";
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