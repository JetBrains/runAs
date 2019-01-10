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
    internal class ToolProcessForWindows : IToolProcess
    {
        [NotNull] private readonly IEnvironment _environment;
        [NotNull] private readonly IFileSystem _fileSystem;
        [NotNull] private readonly Configuration _configuration;
        [NotNull] private readonly List<string> _tempFiles = new List<string>();

        public ToolProcessForWindows(
            [NotNull] IEnvironment environment,
            [NotNull] IFileSystem fileSystem,
            [NotNull] Configuration configuration)
        {
            _environment = environment ?? throw new ArgumentNullException(nameof(environment));
            _fileSystem = fileSystem ?? throw new ArgumentNullException(nameof(fileSystem));
            _configuration = configuration ?? throw new ArgumentNullException(nameof(configuration));
        }

        public Process Process { get; }

        public bool IsSupported(OSType osType) => osType == OSType.Windows;

        public Process CreateProcess()
        {
            var scriptName = Path.Combine(_environment.ToolsPath, "runAs.cmd");
            var settingsArgsFileName = _fileSystem.CreateTempFile(".args", Enumerable.Repeat($"-u:{_configuration.UserName}", 1).Concat(_configuration.RunAsArguments));
            _tempFiles.Add(settingsArgsFileName);
            var commandArgsFileName = _fileSystem.CreateTempFile(".cmd", Enumerable.Repeat(_environment.DotnetPath, 1).Concat(_configuration.CommandArguments));
            _tempFiles.Add(commandArgsFileName);
            var startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                FileName = scriptName,
                Arguments = string.Join(" ", GetArgs(settingsArgsFileName, commandArgsFileName)),
            };

            return new Process { StartInfo = startInfo };
        }

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