namespace JetBrains.runAs.IntegrationTests.Dsl
{
	using System;
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.IO;
	using System.Linq;
	using System.Text;

	internal class RunAsRunner
	{
		public TestSession Run(TestContext ctx)
		{
			var lines = new List<string>();
			var cmdArgs = ctx.CommandLineSetup.Arguments.ToList();
			// cmdArgs.Insert(0, "-l:debug");
			lines.AddRange(ctx.CommandLineSetup.EnvVariables.Select(envVar => $"@SET \"{envVar.Key}={envVar.Value}\""));
			lines.Add($"@pushd \"{ctx.CurrentDirectory}\"");
			lines.Add($"@\"{ctx.CommandLineSetup.ToolName}\" " + string.Join(" ", cmdArgs));
			lines.Add("@set exitCode=%errorlevel%");
			lines.Add("@popd");
			lines.Add("@exit /b %exitCode%");

			var cmd = Path.Combine(ctx.SandboxPath, "run.cmd");
			File.WriteAllText(cmd, string.Join(Environment.NewLine, lines));
            var allOutput = new StringBuilder();
            var output = new StringBuilder();
			var errors = new StringBuilder();
			var process = new Process();
			process.StartInfo.FileName = cmd;
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
			process.StartInfo.CreateNoWindow = true;
			process.StartInfo.RedirectStandardOutput = true;
			process.StartInfo.RedirectStandardError = true;
			process.OutputDataReceived += (sender, args) => { output.AppendLine(args.Data); allOutput.AppendLine(args.Data); };
			process.ErrorDataReceived += (sender, args) => { errors.AppendLine(args.Data); allOutput.AppendLine(args.Data); };
			process.Start();
			process.BeginOutputReadLine();
			process.BeginErrorReadLine();
			process.WaitForExit();
			var outputStr = output.ToString();
			var errorsStr = errors.ToString();
			Console.WriteLine(allOutput);
			return new TestSession(ctx, process.ExitCode, outputStr, errorsStr);
		}
	}
}
