namespace JetBrains.runAs.IntegrationTests
{
	using System;
	using System.Linq;
	using System.Text.RegularExpressions;

	using JetBrains.runAs.IntegrationTests.Dsl;

	using NUnit.Framework;

	using TechTalk.SpecFlow;
	using System.Collections.Generic;

	using TestContext = JetBrains.runAs.IntegrationTests.Dsl.TestContext;

	[Binding]
    public class RunSteps
    {		
		[Given(@"I've added the argument (.+)")]
		public void AddArg(string arg)
		{
			if (string.IsNullOrEmpty(arg))
			{
				return;
			}

			var ctx = ScenarioContext.Current.GetTestContext();
			ctx.CommandLineSetup.Arguments.Add(arg);
		}

		[Given(@"I've added the argument\s+")]
		public void AddEmptyArg()
		{			
		}

		[Given(@"I've defined the (.+) environment variable by the value (.+)")]
		public void AddEnvVar(string name, string value)
		{
			var ctx = ScenarioContext.Current.GetTestContext();
			ctx.CommandLineSetup.EnvVariables[name] = value;
		}

		[When(@"I run RunAs tool")]
        public void RunRunAsTool()
        {
			var ctx = ScenarioContext.Current.GetTestContext();
			var runner = new RunAsRunner();
			var testSession = runner.Run(ctx);
			ctx.TestSession = testSession;
		}

		[Then(@"the exit code should be (.+)")]
		public void VerifyExitCode(string expectedExitCodeRegexp)
		{
			var ctx = ScenarioContext.Current.GetTestContext();
			var regex = new Regex(expectedExitCodeRegexp);
			Assert.IsTrue(regex.Match(ctx.TestSession.ExitCode.ToString()).Success, $"Invalid exit code. Expected is: {expectedExitCodeRegexp}, actual is: {ctx.TestSession.ExitCode}. \nSee {ctx}");

        }

		[Then(@"the output should contain:")]
        public void CheckOutput(Table table)
        {
			var ctx = ScenarioContext.Current.GetTestContext();
			var testSession = ctx.TestSession;
			CheckText(ctx, testSession.Output, table, true);
        }

        [Then(@"the output should not contain:")]
        public void CheckNoOutput(Table table)
        {
            var ctx = ScenarioContext.Current.GetTestContext();
            var testSession = ctx.TestSession;
            CheckText(ctx, testSession.Output, table, false);
        }

        [Then(@"the errors should contain:")]
		public void CheckErrors(Table table)
		{
			var ctx = ScenarioContext.Current.GetTestContext();
			var testSession = ctx.TestSession;
			CheckText(ctx, testSession.Errors, table, true);
		}

        [Then(@"the errors should not contain:")]
        public void CheckNoErrors(Table table)
        {
            var ctx = ScenarioContext.Current.GetTestContext();
            var testSession = ctx.TestSession;
            CheckText(ctx, testSession.Errors, table, false);
        }

        private static void CheckText(TestContext ctx, string text, Table table, bool contains)
		{
			var separator = new[] { Environment.NewLine };
			var lines = new List<string>(text.Split(separator, StringSplitOptions.None));
			var allParrents = new List<Regex>(table.Rows.Select(i => new Regex(i[""], RegexOptions.IgnoreCase | RegexOptions.Singleline | RegexOptions.CultureInvariant | RegexOptions.Compiled)));
		    var notMatched = allParrents.ToList();
            var matched = new List<Regex>();

		    if (contains)
		    {
		        while (lines.Count > 0 && notMatched.Count > 0)
		        {
		            var line = lines[0];
		            lines.RemoveAt(0);
		            if (notMatched[0].IsMatch(line))
		            {
                        notMatched.RemoveAt(0);
		            }
		        }

		        if (notMatched.Any())
		        {
		            Assert.Fail($"Patterns are not matched:\n{string.Join(Environment.NewLine, notMatched)}\nOutput:\n{text}\n\nSee {ctx}");
		        }
		    }
		    else
		    {
                while (lines.Count > 0 && allParrents.Count > 0)
                {
                    var line = lines[0];
                    lines.RemoveAt(0);
                    foreach (var parrent in allParrents)
                    {
                        if (parrent.IsMatch(line))
                        {
                            matched.Add(parrent);
                        }                    
                    }
                }

                if (matched.Any())
                {
                    Assert.Fail($"Patterns are matched:\n{string.Join(Environment.NewLine, matched)}\nOutput:\n{text}\n\nSee {ctx}");
                }
            }  		    
		}
	}
}