namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.IO;
    using System.Linq;
    using IoC;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class ToolProcess : IToolProcess
    {
        [NotNull] private readonly IEnvironment _environment;
        [NotNull] private readonly IFileSystem _fileSystem;
        [NotNull] private readonly Configuration _configuration;
        [NotNull] private readonly List<string> _tempFiles = new List<string>();

        public ToolProcess(
            [NotNull] IEnvironment environment,
            [NotNull] IFileSystem fileSystem,
            [NotNull] Configuration configuration)
        {
            _environment = environment ?? throw new ArgumentNullException(nameof(environment));
            _fileSystem = fileSystem ?? throw new ArgumentNullException(nameof(fileSystem));
            _configuration = configuration ?? throw new ArgumentNullException(nameof(configuration));

            var scriptName = Path.Combine(_environment.ToolsPath, environment.ScriptName);
            var settingsArgsFileName = fileSystem.CreateTempFile(".args", Enumerable.Repeat($"-u:{configuration.UserName}", 1).Concat(configuration.RunAsArguments));
            _tempFiles.Add(settingsArgsFileName);
            var commandArgsFileName = fileSystem.CreateTempFile(".args", Enumerable.Repeat(environment.DotnetPath, 1).Concat(configuration.CommandArguments));
            _tempFiles.Add(commandArgsFileName);
            var startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                FileName = scriptName,
                Arguments = string.Join(" ", GetArgs(settingsArgsFileName, commandArgsFileName)),
            };

            Process = new Process { StartInfo = startInfo };
        }

        public Process Process { get; }

        public void Dispose()
        {
            foreach (var tempFile in _tempFiles)
            {
                try
                {
                    _fileSystem.Delete(tempFile);
                }
                catch
                {
                    // ignored
                }
            }
        }

        private IEnumerable<string> GetArgs(string settingsArgsFileName, string commandArgsFileName)
        {
            yield return $"\"{settingsArgsFileName}\"";
            yield return $"\"{commandArgsFileName}\"";
            yield return $"{_environment.Bitness}";
            yield return $"\"{_configuration.Password}\"";
        }
    }
}