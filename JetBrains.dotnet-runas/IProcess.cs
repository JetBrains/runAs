namespace JetBrains.dotnet_runas
{
    using System;
    using System.Diagnostics;
    using IoC;

    internal interface IProcess: IDisposable
    {
        [NotNull] Process Process { get; }
    }
}
