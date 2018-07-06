namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using IoC;

    internal class Configuration
    {
        public Configuration(
            [NotNull] string userName,
            [NotNull] string password,
            [NotNull] IEnumerable<string> runAsArguments,
            [NotNull] IEnumerable<string> commandArguments,
            Mode mode)
        {
            UserName = userName ?? throw new ArgumentNullException(nameof(userName));
            Password = password ?? throw new ArgumentNullException(nameof(password));
            RunAsArguments = runAsArguments ?? throw new ArgumentNullException(nameof(runAsArguments));
            CommandArguments = commandArguments ?? throw new ArgumentNullException(nameof(commandArguments));
            Mode = mode;
        }

        [NotNull] public string UserName { get; }

        [NotNull] public string Password { get; }

        [NotNull] public IEnumerable<string> RunAsArguments { get; }

        [NotNull] public IEnumerable<string> CommandArguments { get; }

        public Mode Mode { get; }
    }
}