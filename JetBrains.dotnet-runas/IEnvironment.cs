namespace JetBrains.RunAs
{
    using System.Collections.Generic;
    using IoC;

    internal interface IEnvironment
    {
        [NotNull] string ToolsPath { get; }

        [NotNull] string DotnetPath { get; }

        [NotNull] string ScriptName { get; }

        [NotNull] IEnumerable<string> Args { get; }

        int Bitness { get; }
    }
}