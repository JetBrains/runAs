namespace JetBrains.RunAs
{
    using IoC;

    internal interface IConsole
    {
        void WriteStdLine([CanBeNull] string text);

        void WriteErrLine([CanBeNull] string error);
    }
}
