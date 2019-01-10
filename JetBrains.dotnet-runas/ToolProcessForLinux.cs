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
    internal class ToolProcessForLinux : IToolProcess
    {
        [NotNull] private readonly IEnvironment _environment;
        [NotNull] private readonly IFileSystem _fileSystem;
        [NotNull] private readonly Configuration _configuration;
        [NotNull] private readonly List<string> _tempFiles = new List<string>();

        public ToolProcessForLinux(
            [NotNull] IEnvironment environment,
            [NotNull] IFileSystem fileSystem,
            [NotNull] Configuration configuration)
        {
            _environment = environment ?? throw new ArgumentNullException(nameof(environment));
            _fileSystem = fileSystem ?? throw new ArgumentNullException(nameof(fileSystem));
            _configuration = configuration ?? throw new ArgumentNullException(nameof(configuration));
        }

        public bool IsSupported(OSType osType) => osType != OSType.Windows;

        public Process CreateProcess()
        {
            var scriptName = Path.Combine(_environment.ToolsPath, "runAs.sh");
            if (!File.Exists(scriptName))
            {
                throw new InvalidOperationException($"Script \"{scriptName}\" was not found.");
            }

            var settingsArgsFileName = _fileSystem.CreateTempFile(".args", Enumerable.Repeat(_configuration.UserName, 1).Concat(_configuration.RunAsArguments));
            _tempFiles.Add(settingsArgsFileName);
            var args = string.Join(" ", _configuration.CommandArguments.Select(i => $"\"{i}\""));
            var commandArgsFileName = _fileSystem.CreateTempFile(".sh", new [] { "#!/bin/bash", $"\"{_environment.DotnetPath}\" {args}" });
            _tempFiles.Add(commandArgsFileName);
            var startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                FileName = "/bin/bash",
                Arguments = string.Join(" ", GetArgs(scriptName, settingsArgsFileName, commandArgsFileName)),
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

        private IEnumerable<string> GetArgs(string scriptName, string settingsArgsFileName, string commandArgsFileName)
        {
            yield return scriptName;
            yield return $"\"{settingsArgsFileName}\"";
            yield return $"\"{commandArgsFileName}\"";
            yield return $"{_environment.Bitness}";
            yield return $"\"{_configuration.Password}\"";
        }
    }
}