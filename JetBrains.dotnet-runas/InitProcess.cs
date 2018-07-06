namespace JetBrains.RunAs
{
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.IO;
    using IoC;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class InitProcess : IToolProcess
    {
        public InitProcess(
            [NotNull] IEnvironment environment,
            [NotNull] Configuration configuration)
        {
            var scriptName = Path.Combine(environment.ToolsPath, $"init{environment.ScriptExtension}");
            var startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                FileName = scriptName,
                Arguments = configuration.UserName,
            };

            Process = new Process { StartInfo = startInfo };
        }

        public Mode Mode => Mode.Initialize;

        public Process Process { get; }

        public void Dispose()
        {
            //RODO: remove temp dir
        }
    }
}