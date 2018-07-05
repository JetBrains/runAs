namespace JetBrains.dotnet_runas
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using IoC;

    // ReSharper disable once ClassNeverInstantiated.Global
    internal class RunAsConfigurationFactory : IRunAsConfigurationFactory
    {
        [NotNull] private readonly IRunAsEnvironment _runAsEnvironment;

        public RunAsConfigurationFactory([NotNull] IRunAsEnvironment runAsEnvironment)
        {
            _runAsEnvironment = runAsEnvironment ?? throw new ArgumentNullException(nameof(runAsEnvironment));
        }

        public RunAsConfiguration Create()
        {
            var parsinRunAsArgs = true;
            var runAsArgs = new List<string>();
            var commandArguments = new List<string>();
            string userName = null;
            string password = null;
            foreach (var nextArg in _runAsEnvironment.Args)
            {
                var arg = nextArg.TrimStart();
                if (parsinRunAsArgs)
                {
                    if (arg.Length >= 2 && arg[0] == '-' && char.IsLetter(arg[1]))
                    {
                        switch (arg[1])
                        {
                            case 'u':
                                userName = GetValue(arg);
                                break;

                            case 'p':
                                password = GetValue(arg);
                                break;

                            default:
                                runAsArgs.Add(arg);
                                break;
                        }
                    }
                    else
                    {
                        commandArguments.Add(arg);
                        parsinRunAsArgs = false;
                    }
                }
                else
                {
                    commandArguments.Add(arg);
                }
            }

            if (string.IsNullOrWhiteSpace(userName))
            {
                throw new RunAsException("Username was not specified.");
            }

            if (string.IsNullOrEmpty(password))
            {
                throw new RunAsException("Password was not specified.");
            }

            return new RunAsConfiguration(userName, password, runAsArgs.AsReadOnly(), commandArguments.AsReadOnly());
        }

        [NotNull]
        private static string GetValue([NotNull] string arg)
        {
            if (arg == null) throw new ArgumentNullException(nameof(arg));
            return arg.Length <= 3 ? string.Empty : arg.Substring(3, arg.Length - 3);
        }
    }
}
