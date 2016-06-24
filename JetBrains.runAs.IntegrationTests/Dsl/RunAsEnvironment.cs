namespace JetBrains.runAs.IntegrationTests.Dsl
{
	using System;
	using System.IO;

	internal static class RunAsEnvironment
	{
		public static void Prepare(string targetDirectory, CommandLineSetup commandLineSetup)
		{			
#if DEBUG
			const string Build = "Debug";
#else
			const string Build = "Release";
#endif

			var platform = "x64";
#if x86
			platform = "x86";
#endif

			if (Directory.Exists(targetDirectory))
			{
				Directory.Delete(targetDirectory, true);
			}

			Directory.CreateDirectory(targetDirectory);
			
			var binDirectory = Path.Combine(Path.Combine(targetDirectory, "..\\..\\..\\" + Build), platform);
			const string JetbrainsRunasExe = "JetBrains.runAs.exe";
			var runAsSource = Path.Combine(binDirectory, JetbrainsRunasExe);
			var runAsPath = Path.Combine(targetDirectory, JetbrainsRunasExe);
			commandLineSetup.ToolName = Path.GetFileName(runAsPath);
			File.Copy(runAsSource, runAsPath);
		}
	}
}
