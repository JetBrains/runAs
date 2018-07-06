namespace JetBrains.RunAs
{
    using System;
    using System.Diagnostics;
    using IoC;

    internal interface IToolProcess: IDisposable
    {
        [NotNull] Process Process { get; }
    }
}
