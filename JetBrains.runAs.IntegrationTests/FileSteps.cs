namespace JetBrains.runAs.IntegrationTests
{
	using System;
	using System.IO;
	using JetBrains.runAs.IntegrationTests.Dsl;
	using TechTalk.SpecFlow;

	[Binding]
    public class FileSteps
    {
		[Given(@"I have appended the file (.+) by the line (.+)")]
		public void AppendFile(string fileName, string line)
		{
			var ctx = ScenarioContext.Current.GetTestContext();
			File.AppendAllText(Path.Combine(ctx.SandboxPath, fileName.Replace("\"", string.Empty)), line + Environment.NewLine);
		}
	}
}
