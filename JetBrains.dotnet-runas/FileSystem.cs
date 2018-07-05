namespace JetBrains.dotnet_runas
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.CodeAnalysis;
    using System.IO;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class FileSystem : IFileSystem
    {
        public string CreateTempFile(string extension, IEnumerable<string> lines)
        {
            if (extension == null) throw new ArgumentNullException(nameof(extension));
            if (lines == null) throw new ArgumentNullException(nameof(lines));
            var tempFileName = Path.GetTempFileName() + extension;
            File.AppendAllLines(tempFileName, lines);
            return tempFileName;
        }

        public void Delete(string fileName)
        {
            if (fileName == null) throw new ArgumentNullException(nameof(fileName));
            File.Delete(fileName);
        }

        public IEnumerable<string> GetDirectories(string path, string searchPattern)
        {
            if (path == null) throw new ArgumentNullException(nameof(path));
            if (searchPattern == null) throw new ArgumentNullException(nameof(searchPattern));
            return Directory.GetDirectories(path, searchPattern);
        }

        public string GetParent(string path)
        {
            if (path == null) throw new ArgumentNullException(nameof(path));
            return Directory.GetParent(path)?.FullName;
        }
    }
}