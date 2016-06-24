namespace JetBrains.runAs.IntegrationTests.Dsl
{
	using System;
	using System.IO;

	internal class TestContext
	{
		public TestContext()
		{
			SandboxPath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(new Uri(GetType().Assembly.CodeBase).LocalPath), GetSandboxName()));
			CurrentDirectory = SandboxPath;
			CommandLineSetup = new CommandLineSetup { WorkingDirectory = SandboxPath };
			RunAsEnvironment.Prepare(SandboxPath, CommandLineSetup);
		}

		public string SandboxPath { get; private set; }

		public string CurrentDirectory { get; set; }

		public CommandLineSetup CommandLineSetup { get; }

		public TestSession TestSession { get; set; }

		public override string ToString()
		{
			return $"cd \"{SandboxPath}\"";
		}

		private static string GetSandboxName()
		{
			return NUnit.Framework.TestContext.CurrentContext.Test.Name?
				.Replace("(", "_")
				.Replace(",System.String[]", string.Empty)
				.Replace("\"", string.Empty)
				.Replace("\\", string.Empty)
				.Replace(",null", string.Empty)
				.Replace(",", "_")
				.Replace(")", string.Empty) ?? string.Empty;
		}		
	}
}
