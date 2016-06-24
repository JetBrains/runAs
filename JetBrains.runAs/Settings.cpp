#include "stdafx.h"
#include "Settings.h"
#include "ExitCode.h"
#include <sstream>
#include "Args.h"

Settings::Settings(): 
	_exitCodeBase(EXIT_CODE_BASE), _selfTesting(false)
{
}

Settings::Settings(
	const wstring& userName,
	const wstring& domain,
	const wstring& password,
	const wstring& executable,
	const wstring& workingDirectory,
	const int exitCodeBase,
	const list<wstring>& args,
	const list<wstring>& envVars,
	const InheritanceMode& inheritanceMode,
	const IntegrityLevel& integrityLevel,
	const ShowMode& showMode,
	const bool selfTesting)
{
	_userName = userName;
	_domain = domain;
	_password = password;
	_executable = executable;
	_workingDirectory = workingDirectory;
	_exitCodeBase = exitCodeBase;
	_args = list<wstring>(args);
	_envVars = list<wstring>(envVars);
	_inheritanceMode = inheritanceMode;
	_integrityLevel = integrityLevel;
	_showMode = showMode;
	_selfTesting = selfTesting;
}

wstring Settings::GetUserName() const
{	
	return _userName;
}

wstring Settings::GetDomain() const
{
	return _domain;
}


wstring Settings::GetPassword() const
{
	return _password;
}

wstring Settings::GetExecutable() const
{
	return AddQuotes(_executable);	
}

wstring Settings::GetCommandLineArgs() const
{
	wstringstream commandLine;
	for (auto argsIterrator = _args.begin(); argsIterrator != _args.end(); ++argsIterrator)
	{
		if (argsIterrator != _args.begin())
		{
			commandLine << L" ";
		}

		commandLine << AddQuotes(*argsIterrator);
	}
	
	return commandLine.str();
}

wstring Settings::GetCommandLine() const
{
	return GetExecutable() + L" " + GetCommandLineArgs();
}

wstring Settings::GetWorkingDirectory() const
{
	return _workingDirectory;
}

list<wstring> Settings::GetEnvironmentVariables() const
{
	return _envVars;
}

int Settings::GetExitCodeBase() const
{
	return _exitCodeBase;
}

InheritanceMode Settings::GetInheritanceMode() const
{
	return _inheritanceMode;
}

ShowMode Settings::GetShowMode() const
{
	return _showMode;
}

LogLevel Settings::GetLogLevel() const
{
	return _logLevel;
}

void Settings::SetLogLevel(const LogLevel& logLevel)
{
	_logLevel = logLevel;
}

IntegrityLevel Settings::GetIntegrityLevel() const
{
	return _integrityLevel;
}

bool Settings::GetSelfTesting() const
{
	return _selfTesting;
}

wstring Settings::AddQuotes(wstring str)
{
	if (str.find(L' ') != string::npos && str.size() > 0 && !(str[0] == L'\"' && str[str.size() - 1] == L'\"'))
	{
		return L'\"' + str + L'\"';
	}

	return str;
}

wstring Settings::ToString() const
{
	wstringstream text;
	text << L"\t" << ARG_USER_NAME << L":\t\t" << GetUserName();
	text << endl << L"\t" << ARG_DOMAIN << L":\t\t\t" << GetDomain();
	text << endl << L"\t" << ARG_WORKING_DIRECTORY << L":\t" << GetWorkingDirectory();
	for (auto envVarsIterrator = _envVars.begin(); envVarsIterrator != _envVars.end(); ++envVarsIterrator)
	{
		text << endl << L"\t" << ARG_ENV_VAR << L":\t\t" << *envVarsIterrator;
	}
	
	text << endl << L"\t" << ARG_EXIT_CODE_BASE << L":\t\t" << GetExitCodeBase();
	text << endl << L"\t" << ARG_INTEGRITY_LEVEL << L":\t" << GetIntegrityLevel();
	text << endl << L"\t" << ARG_INHERITANCE_MODE << L":\t" << GetInheritanceMode();
	text << endl << L"\t" << ARG_SHOW_MODE << L":\t\t" << GetShowMode();
	text << endl << L"\t" << ARG_SELF_TESTING << L":\t\t" << GetSelfTesting();
	text << endl << L"\t" << ARG_EXECUTABLE << L":\t\t" << GetExecutable();
	text << endl << L"\t" << ARG_EXIT_COMMAND_LINE_ARGS << L":\t" << GetCommandLineArgs();
	return text.str();
}
