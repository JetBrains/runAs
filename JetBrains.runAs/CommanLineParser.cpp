#include "stdafx.h"
#include "CommanLineParser.h"
#include <fstream>
#include <sstream>
#include <regex>
#include "Settings.h"
#include <list>
#include "Result.h"
#include "ErrorCode.h"
#include "ExitCode.h"
#include "Args.h"
#include "StringUtilities.h"

static const wregex ArgRegex = wregex(L"\\s*-\\s*(\\w+)\\s*(:\\s*(.+)|\\s*)\\s*$");
static const wregex UserRegex = wregex(L"^([^@\\\\]+)@([^@\\\\]+)$|^([^@\\\\]+)\\\\([^@\\\\]+)$|(^[^@\\\\]+$)");
static const wstring TrueStr = L"true";
static const wstring FalseStr = L"false";

Result<Settings> CommanLineParser::TryParse(const list<wstring>& args, ExitCode* exitCodeBase, LogLevel* logLevel) const
{
	auto actualArgs = list<wstring>(args);
	wstring userName;
	wstring domain;
	wstring password;
	wstring executable;
	wstring workingDirectory;
	*exitCodeBase = EXIT_CODE_BASE;
	list<wstring> commandLineArgs;
	list<wstring> envVars;
	wstring inheritanceMode = INHERITANCE_MODE_AUTO;
	auto argsMode = 0; // 0 - gets tool args, 1 - gets executable, 2 - gets cmd args
	*logLevel = LOG_LEVEL_NORMAL;
	IntegrityLevel integrityLevel = INTEGRITY_LEVEL_DEFAULT;
	ShowMode showMode = SHOW_MODE_HIDE;
	auto selfTesting = false;
	
	while (actualArgs.size() > 0)
	{
		auto arg = *actualArgs.begin();		

		if (argsMode == 2)
		{
			commandLineArgs.push_back(arg);
			actualArgs.erase(actualArgs.begin());
			continue;
		}

		if (argsMode == 1)
		{
			executable = arg;
			argsMode = 2;
			actualArgs.erase(actualArgs.begin());
			continue;
		}		

		wsmatch matchResult;
		if (!regex_search(arg, matchResult, ArgRegex))
		{
			argsMode = 1;
			continue;
		}

		actualArgs.erase(actualArgs.begin());

		auto argName = matchResult._At(1).str();
		auto argValue = matchResult._At(3).str();		
		auto argNameInLowCase = StringUtilities::Convert(argName, tolower);
		auto argValueInLowCase = StringUtilities::Convert(argValue, tolower);

		// User name like SomeUserName or domain\SomeUserName or SomeUserName@domain
		if (argNameInLowCase == L"u")
		{
			if (!regex_search(argValue, matchResult, UserRegex))
			{
				return Error(L"TryParse", 0, ERROR_CODE_INVALID_USAGE, L"Invalid format of user name \"" + argValue + L"\"");
			}

			userName = matchResult._At(1).str();
			if (userName == L"")
			{
				userName = matchResult._At(4).str();
			}

			if (userName == L"")
			{
				userName = matchResult._At(5).str();
			}

			domain = matchResult._At(2).str();
			if (domain == L"")
			{
				domain = matchResult._At(3).str();
			}

			if (domain == L"")
			{
				domain = L".";
			}

			continue;
		}

		// Password
		if (argNameInLowCase == L"p")
		{
			password = argValue;
			continue;
		}		

		// Working directory
		if (argNameInLowCase == L"w")
		{
			workingDirectory = argValue;
			continue;
		}

		// Exit code base
		if (argNameInLowCase == L"b")
		{
			*exitCodeBase = stoi(argValue);
			continue;
		}		

		// Configuration file
		if (argNameInLowCase == L"c")
		{
			// Extract args from file
			auto configFileName = argValue;
			wifstream configFile;
			configFile.open(configFileName);
			if (!configFile.is_open())
			{
				return Error(L"TryParse", 0, ERROR_CODE_INVALID_USAGE, L"Unable to open file: \"" + configFileName + L"\"");
			}

			wstring line;
			auto curElement = actualArgs.begin();
			while (getline(configFile, line))
			{
				curElement = actualArgs.insert(curElement, line);
				++curElement;
			}

			configFile.close();
			continue;
		}

		// Inherite environment
		if (argNameInLowCase == L"i")
		{		
			if (TryGetValue(InheritanceModes, argValueInLowCase, &inheritanceMode))
			{
				continue;
			}			
		}

		// Log level
		if (argNameInLowCase == L"l")
		{
			if (TryGetValue(LogLevels, argValueInLowCase, logLevel))
			{
				continue;
			}
		}

		// Integrity level
		if (argNameInLowCase == L"il")
		{
			if (TryGetValue(IntegrityLevels, argValueInLowCase, &integrityLevel))
			{
				continue;
			}			
		}

		// Environment variable
		if (argNameInLowCase == L"e")
		{
			envVars.push_back(argValue);
			continue;
		}

		// Show mode
		if (argNameInLowCase == L"s")
		{
			if (TryGetValue(ShowModes, argValueInLowCase, &showMode))
			{
				continue;
			}			
		}

		// Selft test mode
		if (argNameInLowCase == L"t")
		{			
			selfTesting = true;
			continue;
		}

		return Error(L"TryParse", 0, ERROR_CODE_INVALID_USAGE, L"Invalid argument \"" + argName + L"\"");
	}	

	if (workingDirectory == L"")
	{
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		workingDirectory = wstring(path);
	}

	list<wstring> emptyArgs;
	if (userName == L"")
	{		
		emptyArgs.push_back(ARG_USER_NAME);
	}	

	if (executable == L"")
	{
		emptyArgs.push_back(ARG_EXECUTABLE);		
	}

	if (!selfTesting && emptyArgs.size() > 0)
	{
		wstringstream details;
		details << L"The argument(s):";
		for (auto emptyArgsIterrator = emptyArgs.begin(); emptyArgsIterrator != emptyArgs.end(); ++emptyArgsIterrator)
		{
			details << L" \"" << *emptyArgsIterrator << L"\"";
		}

		details << L" should not be empty.";
		return Error(L"TryParse", 0, ERROR_CODE_INVALID_USAGE, details.str());
	}	

	auto settings = Settings(
		userName,
		domain,
		password,
		executable,
		workingDirectory,
		*exitCodeBase,
		commandLineArgs,
		envVars,
		inheritanceMode,
		integrityLevel,		
		showMode,
		selfTesting);

	settings.SetLogLevel(*logLevel);
	return settings;
}

bool CommanLineParser::TryGetValue(const list<wstring>& values, const wstring& str, wstring* value) const
{	
	auto strInLowerCase = StringUtilities::Convert(str, tolower);
	for (auto valueIterrator = values.begin(); valueIterrator != values.end(); ++valueIterrator)
	{
		auto valueFromListInLowerCase = StringUtilities::Convert(*valueIterrator, tolower);
		if(strInLowerCase == valueFromListInLowerCase)
		{
			*value = *valueIterrator;
			return true;
		}
	}

	return false;
}
