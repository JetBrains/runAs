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
            try
            {
                using (var container = Container.Create().Using<IoCConfiguration>())
                {
                    return container.Resolve<Program>().Run();
                }
            }
            catch (ToolException parseException)
            {
                System.Console.Error.WriteLine(parseException.Message);
                System.Console.WriteLine("Usage: dotnet runas -u:username -p:password [dotnet CLI command and arguments]");
                System.Console.WriteLine("Example: dotnet runas -u:TestUser -p:MyPassword test MyTest.csproj -c release");
                return 1;
            }
        }

        [NotNull] private readonly IProcessRunner _processRunner;

        internal Program([NotNull] IProcessRunner processRunner)
        {
            _processRunner = processRunner ?? throw new ArgumentNullException(nameof(processRunner));
        }

        private int Run()
        {
            return _processRunner.Run();
        }
    }
}
