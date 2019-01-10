namespace JetBrains.RunAs
{
    using System;
    using System.Diagnostics;
    using IoC;

    internal interface IToolProcess: IDisposable
    {
        bool IsSupported(OSType osType);

        [NotNull] Process CreateProcess();
    }
}
