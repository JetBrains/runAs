namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using IoC;

    internal class IoCConfiguration: IConfiguration
    {
        public IEnumerable<IDisposable> Apply(IContainer container)
        {
            yield return container.Bind<Program>().As(Lifetime.Singleton).To();
            yield return container.Bind<IProcessRunner>().As(Lifetime.Singleton).To<ProcessRunner>();
            yield return container.Bind<IConfigurationFactory>().As(Lifetime.Singleton).To<ConfigurationFactory>();
            yield return container.Bind<IConsole>().As(Lifetime.Singleton).To<Console>();
            yield return container.Bind<IEnvironment>().As(Lifetime.Singleton).To<ToolEnvironment>();
            yield return container.Bind<Configuration>().As(Lifetime.Singleton).To(ctx => ctx.Container.Inject<IConfigurationFactory>().Create());
            yield return container.Bind<IFileSystem>().As(Lifetime.Singleton).To<FileSystem>();
            yield return container.Bind<IToolProcess>().Tag(Mode.Run).To<ToolProcess>();
            yield return container.Bind<IToolProcess>().Tag(Mode.Initialize).To<InitProcess>();
        }
    }
}
