namespace JetBrains.dotnet_runas
{
    using IoC;

    internal interface IConsole
    {
        void WriteStdLine([CanBeNull] string text);

        void WriteErrLine([CanBeNull] string error);
    }
}
