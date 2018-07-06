namespace JetBrains.RunAs
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.CodeAnalysis;
    using System.IO;
    using System.Linq;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using IoC;
    using Microsoft.DotNet.Cli.Utils;

    [SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
    internal class ToolEnvironment : IEnvironment
    {
        public ToolEnvironment([NotNull] IFileSystem fileSystem)
        {
            if (fileSystem == null) throw new ArgumentNullException(nameof(fileSystem));
            DotnetPath = new Muxer().MuxerPath;
            var location = GetType().GetTypeInfo().Assembly.Location;
            var curDirectory = Path.GetDirectoryName(location);
            string[] targetDirectory;
            do
            {
                targetDirectory = fileSystem.GetDirectories(curDirectory, "Any").ToArray();
                curDirectory = fileSystem.GetParent(curDirectory);
            }
            while (curDirectory !=null && targetDirectory.Length == 0);

            if (targetDirectory.Length != 1)
            {
                throw new ToolException("Cannot find tools.");
            }

            ToolsPath = targetDirectory[0];
        }

        public string ToolsPath { get; }

        public string DotnetPath { get; }

        public string ScriptName
        {
            get
            {
                if (OsPlatform == OSPlatform.Windows)
                {
                    return "runAs.cmd";
                }

                if (OsPlatform == OSPlatform.Linux)
                {
                    return "runAs.sh";
                }
                
                if (OsPlatform == OSPlatform.OSX)
                {
                    return "runAs_mac.sh";
                }

                throw new ToolException("Unsupported OS.");
            }
        }

        public IEnumerable<string> Args => Environment.GetCommandLineArgs().Skip(1);

        public int Bitness
        {
            get
            {
                switch (RuntimeInformation.OSArchitecture)
                {
                    case Architecture.X64:
                        return 64;

                    case Architecture.X86:
                        return 32;

                    default:
                        return 64;
                }
            }
        }

        private static OSPlatform OsPlatform
        {
            get
            {
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    return OSPlatform.Windows;
                }

                if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
                {
                    return OSPlatform.Linux;
                }

                if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
                {
                    return OSPlatform.OSX;
                }

                throw new ToolException("Unsupported OS.");
            }
        }
    }
}
