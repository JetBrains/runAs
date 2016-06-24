namespace JetBrains.runAs.IntegrationTests.Dsl
{
	using System.Collections.Generic;

	internal class CommandLineSetup
	{
		public CommandLineSetup()
		{
			Arguments = new List<string>();
			EnvVariables = new Dictionary<string, string>();			
		}

		public string ToolName { get; set; }

		public string WorkingDirectory { get; set; }

		public IList<string> Arguments { get; private set; }

		public IDictionary<string, string> EnvVariables { get; private set; }
	}
}
