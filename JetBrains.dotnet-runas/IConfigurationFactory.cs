namespace JetBrains.RunAs
{
    using IoC;

    internal interface IConfigurationFactory
    {
        [NotNull] Configuration Create();
    }
}