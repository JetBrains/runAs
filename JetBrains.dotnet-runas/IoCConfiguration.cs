namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using IoC;
    using static IoC.Lifetime;

    internal class IoCConfiguration: IConfiguration
    {
        public IEnumerable<IDisposable> Apply([NotNull] IContainer container)
        {
            if (container == null) throw new ArgumentNullException(nameof(container));
            yield return container.Bind<Program>().As(Singleton).To();
            yield return container.Bind<IProcessRunner>().As(Singleton).To<ProcessRunner>();
            yield return container.Bind<IConfigurationFactory>().As(Singleton).To<ConfigurationFactory>();
            yield return container.Bind<IConsole>().As(Singleton).To<Console>();
            yield return container.Bind<IEnvironment>().As(Singleton).To<ToolEnvironment>();
            yield return container.Bind<Configuration>().As(Singleton).To(ctx => ctx.Container.Inject<IConfigurationFactory>().Create());
            yield return container.Bind<IFileSystem>().As(Singleton).To<FileSystem>();
            yield return container.Bind<IToolProcess>().Tag(OSType.Windows).To<ToolProcessForWindows>();
            yield return container.Bind<IToolProcess>().Tag(OSType.Linux).To<ToolProcessForLinux>();
        }
    }
}
