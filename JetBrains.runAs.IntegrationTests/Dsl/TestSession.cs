namespace JetBrains.runAs.IntegrationTests.Dsl
{
	internal class TestSession
	{
		private readonly TestContext _context;

		public TestSession(TestContext context, int exitCode, string output, string errors)
		{
			_context = context;
			ExitCode = exitCode;
			Output = output;
			Errors = errors;
		}

		public int ExitCode { get; }

		public string Output { get; }

		public string Errors { get; }

		public override string ToString()
		{
			return $"cd {_context.SandboxPath}";
		}
	}
}
