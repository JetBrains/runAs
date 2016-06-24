#include "stdafx.h"
#include "CommanLineParser.h"
#include "Settings.h"
#include "HelpUtilities.h"
#include "ErrorCode.h"
#include "Result.h"
#include <iostream>
#include "Runner.h"
#include "Trace.h"
#include <iomanip>

wstring GetStringValue(wstring value)
{
	if (value.size() == 0)
	{
		return L"empty";
	}

	return value;
}

int _tmain(int argc, _TCHAR *argv[]) {
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOOPENFILEERRORBOX);	
	list<wstring> args;
	for (auto argIndex = 1; argIndex < argc; argIndex++)
	{
		args.push_back(argv[argIndex]);
	}

	list<Result<ExitCode>> results;
	Settings settings;
	ExitCode exitCodeBase = EXIT_CODE_BASE;
	LogLevel logLevel;
	Console console;
	wstring endl = L"\n\r";
	try
	{
		CommanLineParser commanLineParser;
		auto settingsResult = commanLineParser.TryParse(args, &exitCodeBase, &logLevel);
		if (!settingsResult.HasError())
		{
			settings = settingsResult.GetResultValue();
				
			// Show header
			Trace trace(settings.GetLogLevel());
			trace < HelpUtilities::GeTitle();

			Runner runner;
			trace < L"main::Run starting";			
			results.push_back(runner.Run(settings));
			trace < L"main::Run finished";
		}
		else
		{
			results.push_back(settingsResult.GetError());
		}
	}
	catch(...)
	{
		results.push_back(Error());
	}	

	Trace trace(settings.GetLogLevel());
	trace < L"main::Create results";	

	auto hasError = results.back().HasError();
	auto showFullInfo = (hasError && (logLevel == LOG_LEVEL_NORMAL || logLevel == LOG_LEVEL_DEBUG)) || (!hasError && logLevel == LOG_LEVEL_DEBUG);	
	if (showFullInfo)
	{
		// Show header
		console << HelpUtilities::GetHeader();

		// Show arguments
		console << endl << endl << L"Argument(s):";
		if (args.size() == 0)
		{
			console << L" empty";
		}
		else
		{
			for (auto argsIterrator = args.begin(); argsIterrator != args.end(); ++argsIterrator)
			{
				console << L" " << *argsIterrator;
			}
		}

		// Show settings
		console << endl << endl << L"Settings:" << endl << settings.ToString();
		if (results.back().GetError().GetCode() == ERROR_CODE_INVALID_USAGE)
		{
			console << endl << HelpUtilities::GetHelp();
		}
	}

	if (!hasError)
	{
		trace < L"Exit code: ";
		trace << results.back().GetResultValue();
		return results.back().GetResultValue();
	}

	if (logLevel != LOG_LEVEL_OFF)
	{
		if (results.back().GetError().GetDescription() != L"")
		{
			wcerr << endl << endl << L"Error: " + results.back().GetError().GetDescription();
		}
	}	

	if (showFullInfo)
	{
		wcerr << endl << results.back().GetError().GetTarget() << L" returns the Win32 error 0x" << hex << setw(8) << setfill(L'0') << results.back().GetError().GetWin32Error() << L".";
	}

	auto exitCode = exitCodeBase > 0 ? exitCodeBase + results.back().GetError().GetCode() : exitCodeBase - results.back().GetError().GetCode();
	trace << endl << L"Error code: " << exitCode;
	return exitCode;
}
