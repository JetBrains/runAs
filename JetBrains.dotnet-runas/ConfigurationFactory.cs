namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using IoC;

    // ReSharper disable once ClassNeverInstantiated.Global
    internal class ConfigurationFactory : IConfigurationFactory
    {
        [NotNull] private readonly IEnvironment _environment;

        public ConfigurationFactory([NotNull] IEnvironment environment)
        {
            _environment = environment ?? throw new ArgumentNullException(nameof(environment));
        }

        public Configuration Create()
        {
            var parsinRunAsArgs = true;
            var runAsArgs = new List<string>();
            var commandArguments = new List<string>();
            var userName = string.Empty;
            var password = string.Empty;
            foreach (var nextArg in _environment.Args)
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
                throw new ToolException("Username was not specified.");
            }

            var initMode = string.IsNullOrEmpty(password) && runAsArgs.Count == 0 && commandArguments.Count == 0;

            if (!initMode && string.IsNullOrWhiteSpace(password))
            {
                throw new ToolException("Password was not specified.");
            }

            return new Configuration(
                userName,
                password,
                runAsArgs.AsReadOnly(),
                commandArguments.AsReadOnly(),
                initMode ? Mode.Initialize : Mode.Run);
        }

        [NotNull]
        private static string GetValue([NotNull] string arg)
        {
            if (arg == null) throw new ArgumentNullException(nameof(arg));
            return arg.Length <= 3 ? string.Empty : arg.Substring(3, arg.Length - 3);
        }
    }
}
