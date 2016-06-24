#pragma once
#include <list>
#include "LogLevel.h"
#include "InheritanceMode.h"
#include "IntegrityLevel.h"
#include "ShowMode.h"

class Settings
{
	wstring _userName = L"";
	wstring _domain = L"";
	wstring _password = L"";
	wstring _executable = L"";
	wstring _workingDirectory = L"";
	list<wstring> _args = {};
	list<wstring> _envVars = {};
	int _exitCodeBase = 0;
	InheritanceMode _inheritanceMode = INHERITANCE_MODE_AUTO;
	LogLevel _logLevel = LOG_LEVEL_NORMAL;
	IntegrityLevel _integrityLevel = INTEGRITY_LEVEL_DEFAULT;
	ShowMode _showMode = SHOW_MODE_HIDE;
	bool _selfTesting;

	static wstring AddQuotes(wstring str);

public:
	Settings();
	Settings(
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
		const bool selfTesting);

	wstring GetUserName() const;
	wstring GetDomain() const;
	wstring GetPassword() const;
	wstring GetExecutable() const;
	wstring GetCommandLineArgs() const;
	wstring GetCommandLine() const;
	wstring GetWorkingDirectory() const;
	list<wstring> GetEnvironmentVariables() const;
	int GetExitCodeBase() const;
	InheritanceMode GetInheritanceMode() const;
	ShowMode GetShowMode() const;
	LogLevel GetLogLevel() const;
	void SetLogLevel(const LogLevel& logLevel);
	IntegrityLevel GetIntegrityLevel() const;
	bool GetSelfTesting() const;
	wstring ToString() const;
};
