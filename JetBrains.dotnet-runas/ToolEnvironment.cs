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
        private readonly string _baseToolsPath;

        public ToolEnvironment([NotNull] IFileSystem fileSystem)
        {
            if (fileSystem == null) throw new ArgumentNullException(nameof(fileSystem));
            DotnetPath = new Muxer().MuxerPath;
            var location = GetType().GetTypeInfo().Assembly.Location;
            var curDirectory = Path.GetDirectoryName(location);
            string[] targetDirectory;
            do
            {
                targetDirectory = fileSystem.GetDirectories(Path.Combine("..", Path.Combine("..", curDirectory)), "common").ToArray();
                curDirectory = fileSystem.GetParent(curDirectory);
            }
            while (curDirectory !=null && targetDirectory.Length == 0);

            if (targetDirectory.Length != 1)
            {
                throw new ToolException("Cannot find tools.");
            }

            _baseToolsPath = targetDirectory[0];
        }

        public string ToolsPath => Path.Combine(_baseToolsPath, OsType.ToString());

        public string DotnetPath { get; }

        public string ScriptExtension
        {
            get
            {
                switch (OsType)
                {
                    case OSType.Windows:
                        return ".cmd";

                    case OSType.Linux:
                    case OSType.OSX:
                        return ".cmd";

                    default:
                        throw new ToolException("Unsupported OS.");
                }
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

        private static OSType OsType
        {
            get
            {
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    return OSType.Windows;
                }

                if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
                {
                    return OSType.Linux;
                }

                if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
                {
                    return OSType.OSX;
                }

                throw new ToolException("Unsupported OS.");
            }
        }
    }
}
