namespace JetBrains.dotnet_runas
{
    using System.Collections.Generic;
    using IoC;

    internal interface IFileSystem
    {
        [NotNull] string CreateTempFile([NotNull] string extension, [NotNull][ItemNotNull] IEnumerable<string> lines);

        void Delete([NotNull] string fileName);

        [NotNull] IEnumerable<string> GetDirectories([NotNull] string path, [NotNull] string searchPattern);

        [CanBeNull] string GetParent([NotNull] string path);
    }
}
