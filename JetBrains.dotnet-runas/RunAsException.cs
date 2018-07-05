namespace JetBrains.dotnet_runas
{
    using System;

    internal class RunAsException: Exception
    {
        public RunAsException(string message) : base(message)
        {
        }
    }
}
