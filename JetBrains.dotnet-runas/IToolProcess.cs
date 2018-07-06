namespace JetBrains.RunAs
{
    using System;
    using System.Diagnostics;
    using IoC;

    internal interface IToolProcess: IDisposable
    {
        Mode Mode { get; }

        [NotNull] Process Process { get; }
    }
}
