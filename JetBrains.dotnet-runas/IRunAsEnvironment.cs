namespace JetBrains.dotnet_runas
{
    using System.Collections.Generic;
    using IoC;

    internal interface IRunAsEnvironment
    {
        [NotNull] string ToolsPath { get; }

        [NotNull] string DotnetPath { get; }

        [NotNull] string ScriptName { get; }

        [NotNull] IEnumerable<string> Args { get; }

        int Bitness { get; }
    }
}