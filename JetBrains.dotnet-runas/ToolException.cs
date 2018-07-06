namespace JetBrains.RunAs
{
    using System;

    internal class ToolException: Exception
    {
        public ToolException(string message) : base(message)
        {
        }
    }
}
