namespace JetBrains.dotnet_runas
{
    using System;
    using System.Diagnostics.CodeAnalysis;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class Console : IConsole
    {
        public void WriteStdLine(string text)
        {
            if (text == null)
            {
                return;
            }

            System.Console.Out.WriteLine(text);
        }

        public void WriteErrLine(string error)
        {
            if (error == null)
            {
                return;
            }

            var color = System.Console.ForegroundColor;
            System.Console.ForegroundColor = ConsoleColor.Red;
            try
            {
                System.Console.Error.WriteLine(error);
            }
            finally
            {
                System.Console.ForegroundColor = color;
            }
        }
    }
}