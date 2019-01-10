namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.Linq;
    using IoC;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class ProcessRunner : IProcessRunner, IDisposable
    {
        [NotNull] private readonly IToolProcess _process;
        [NotNull] private readonly IConsole _console;

        public ProcessRunner(
            [NotNull] IEnumerable<IToolProcess> processes,
            [NotNull] IConsole console,
            [NotNull] IEnvironment environment)
        {
            if (environment == null) throw new ArgumentNullException(nameof(environment));
            _process = (processes ?? throw new ArgumentNullException(nameof(processes))).Single(i => i.IsSupported(environment.OsType));            
            _console = console ?? throw new ArgumentNullException(nameof(console));
        }

        public int Run()
        {
            void OnOutputDataReceived(object sender, DataReceivedEventArgs args) => _console.WriteStdLine(args.Data);
            void OnErrorDataReceived(object sender, DataReceivedEventArgs args) => _console.WriteErrLine(args.Data);

            var process = _process.CreateProcess();
            process.OutputDataReceived += OnOutputDataReceived;
            process.ErrorDataReceived += OnErrorDataReceived;

            process.Start();
            try
            {
                process.BeginOutputReadLine();
                process.BeginErrorReadLine();
            }
            finally
            {
                process.WaitForExit();
                process.OutputDataReceived -= OnOutputDataReceived;
                process.ErrorDataReceived -= OnErrorDataReceived;
            }

            return process.ExitCode;
        }

        public void Dispose()
        {
            _process.Dispose();
        }
    }
}
