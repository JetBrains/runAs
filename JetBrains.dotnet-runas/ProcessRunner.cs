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
        [NotNull] private readonly IConsole _сonsole;

        public ProcessRunner(
            [NotNull] IEnumerable<IToolProcess> processes,
            [NotNull] IConsole сonsole,
            [NotNull] Configuration configuration)
        {
            _process = (processes ?? throw new ArgumentNullException(nameof(processes))).Single(i => i.Mode == configuration.Mode);
            _сonsole = сonsole ?? throw new ArgumentNullException(nameof(сonsole));
        }

        public int Run()
        {
            void OnOutputDataReceived(object sender, DataReceivedEventArgs args) => _сonsole.WriteStdLine(args.Data);
            void OnErrorDataReceived(object sender, DataReceivedEventArgs args) => _сonsole.WriteErrLine(args.Data);

            var process = _process.Process;
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
