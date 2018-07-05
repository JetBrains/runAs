namespace JetBrains.dotnet_runas
{
    using System;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using IoC;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class ProcessRunner : IProcessRunner, IDisposable
    {
        [NotNull] private readonly IProcess _process;
        [NotNull] private readonly IConsole _сonsole;

        public ProcessRunner(
            [NotNull] IProcess process,
            [NotNull] IConsole сonsole)
        {
            _process = process ?? throw new ArgumentNullException(nameof(process));
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
