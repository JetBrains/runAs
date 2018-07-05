namespace JetBrains.dotnet_runas
{
    using System;
    using System.Collections.Generic;
    using IoC;
    using static IoC.Lifetime;

    internal class IoCConfiguration: IConfiguration
    {
        public IEnumerable<IDisposable> Apply(IContainer container)
        {
            yield return container.Bind<Program>().As(Singleton).To();
            yield return container.Bind<IProcessRunner>().As(Singleton).To<ProcessRunner>();
            yield return container.Bind<IRunAsConfigurationFactory>().As(Singleton).To<RunAsConfigurationFactory>();
            yield return container.Bind<IConsole>().As(Singleton).To<Console>();
            yield return container.Bind<IRunAsEnvironment>().As(Singleton).To<RunAsEnvironment>();
            yield return container.Bind<RunAsConfiguration>().As(Singleton).To(ctx => ctx.Container.Inject<IRunAsConfigurationFactory>().Create());
            yield return container.Bind<IFileSystem>().As(Singleton).To<FileSystem>();
            yield return container.Bind<IProcess>().As(Singleton).To<RunAsProcess>();
        }
    }
}
