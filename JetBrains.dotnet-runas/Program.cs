namespace JetBrains.RunAs
{
    using System;
    using System.Diagnostics.CodeAnalysis;
    using IoC;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    public class Program
    {
        public static int Main()
        {
            using (var container = Container.Create().Using<IoCConfiguration>())
            {
                return container.Resolve<Program>().Run();
            }
        }

        [NotNull] private readonly IProcessRunner _processRunner;
        [NotNull] private readonly IConsole _console;

        internal Program(
            [NotNull] IProcessRunner processRunner,
            [NotNull] IConsole console)
        {
            _processRunner = processRunner ?? throw new ArgumentNullException(nameof(processRunner));
            _console = console ?? throw new ArgumentNullException(nameof(console));
        }

        private int Run()
        {
            try
            {
                return _processRunner.Run();
            }
            catch (ToolException parseException)
            {
                _console.WriteErrLine(parseException.Message);
                return 1;
            }
        }
    }
}
