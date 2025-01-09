namespace Tmplt.Core;

public class TemplateManager
    {
        public void CreateFromTemplate(string targetPath, Template template)
        {
            if (!Directory.Exists(targetPath))
                Directory.CreateDirectory(targetPath);

            foreach (var item in template.Items)
            {
                if (item.IsFolder)
                {
                    Directory.CreateDirectory(Path.Combine(targetPath, item.Path));
                    Console.WriteLine($"Created directory: {item.Path}");
                }
                else
                {
                    File.WriteAllText(Path.Combine(targetPath, item.Path), item.Content);
                    Console.WriteLine($"Created file: {item.Path}");
                }
            }
        }

        public Template FromDirectory(string name, string directory)
        {
            if (!Directory.Exists(directory))
                throw new DirectoryNotFoundException($"The directory '{directory}' does not exist.");

            var template = new Template
            {
                Name = name
            };

            var items = new List<TemplateItem>();
            AddFilesAndDirectories(directory, items);
            template.Items = items.OrderByDescending(i => i.IsFolder).ToList();

            return template;
        }

        private void AddFilesAndDirectories(string directory, List<TemplateItem> items)
        {
            var files = Directory.GetFiles(directory, "*.*", SearchOption.AllDirectories);
            foreach (var file in files)
            {
                try
                {
                    var content = File.ReadAllText(file);
                    var path = Path.GetRelativePath(directory, file);
                    items.Add(new TemplateItem(path, content, false));
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Failed to read file {file}: {ex.Message}");
                }
            }

            var directories = Directory.GetDirectories(directory, "*", SearchOption.AllDirectories);
            foreach (var dir in directories)
            {
                try
                {
                    var path = Path.GetRelativePath(directory, dir);
                    items.Add(new TemplateItem(path, "", true));
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Failed to read directory {dir}: {ex.Message}");
                }
            }
        }
    }