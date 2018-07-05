namespace JetBrains.dotnet_runas
{
    using IoC;

    internal interface IRunAsConfigurationFactory
    {
        [NotNull] RunAsConfiguration Create();
    }
}