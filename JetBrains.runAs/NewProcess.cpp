#include "stdafx.h"
#include "NewProcess.h"
#include "Trace.h"
#include "SecurityManager.h"
#include "StringBuffer.h"
#include "ProcessTracker.h"
#include "ShowModeConverter.h"
#include "Environment.h"

Result<ExitCode> NewProcess::Run(const Settings& settings, ProcessTracker& processTracker) const
{
	Trace trace(settings.GetLogLevel());

	Handle processToken(L"Process token");
	if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &processToken))
	{
		return Error(L"OpenProcessToken");
	}

	SecurityManager securityManager;
	auto currentIntegrityLevelResult = securityManager.GetIntegrityLevel(trace, processToken);
	if (currentIntegrityLevelResult.HasError())
	{
		return currentIntegrityLevelResult.GetError();
	}

	if (settings.GetIntegrityLevel() != INTEGRITY_LEVEL_AUTO || settings.GetIntegrityLevel() != currentIntegrityLevelResult.GetResultValue())
	{
		auto setIntegrityLevelResult = securityManager.SetIntegrityLevel(settings.GetIntegrityLevel(), processToken, trace);
		if (setIntegrityLevelResult.HasError())
		{
			return Result<ExitCode>(setIntegrityLevelResult.GetError());
		}
	}
	
	StringBuffer userName(settings.GetUserName());
	StringBuffer domain(settings.GetDomain());
	StringBuffer password(settings.GetPassword());
	StringBuffer workingDirectory(settings.GetWorkingDirectory());
	StringBuffer commandLine(settings.GetCommandLine());

	SECURITY_ATTRIBUTES processSecAttributes = {};
	processSecAttributes.nLength = sizeof(SECURITY_DESCRIPTOR);
	processSecAttributes.bInheritHandle = true;

	SECURITY_ATTRIBUTES threadSecAttributes = {};
	threadSecAttributes.nLength = 0;
	threadSecAttributes.bInheritHandle = false;
	
	STARTUPINFO startupInfo = {};
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = ShowModeConverter::ToShowWindowFlag(settings.GetShowMode());

	trace < L"ProcessTracker::InitializeConsoleRedirection";
	auto error = processTracker.InitializeConsoleRedirection(processSecAttributes, startupInfo);
	if (error.HasError())
	{
		return Result<ExitCode>(error.GetError());
	}

	PROCESS_INFORMATION processInformation = {};

	auto newProcessEnvironmentResult = GetEnvironment(settings, trace);
	if (newProcessEnvironmentResult.HasError())
	{
		return Result<ExitCode>(newProcessEnvironmentResult.GetError());
	}

	trace < L"::CreateProcess";
	if (!CreateProcess(
		nullptr,
		commandLine.GetPointer(),
		&processSecAttributes,
		&threadSecAttributes,
		true,
		CREATE_UNICODE_ENVIRONMENT,
		newProcessEnvironmentResult.GetResultValue().CreateEnvironment(),
		workingDirectory.GetPointer(),
		&startupInfo,
		&processInformation))
	{
		return Error(L"CreateProcess");		
	}

	// ReSharper disable CppInitializedValueIsAlwaysRewritten
	// ReSharper disable CppEntityAssignedButNoRead

	auto processHandle = Handle(L"Service Process");	
	processHandle = processInformation.hProcess;

	auto threadHandle = Handle(L"Thread");
	threadHandle = processInformation.hThread;

	return processTracker.WaiteForExit(processInformation.hProcess, trace);	
}

Result<Environment> NewProcess::GetEnvironment(const Settings& settings, Trace& trace) const
{
	auto callingProcessEnvironmentResult = Environment::CreateForCurrentProcess(trace);
	if (callingProcessEnvironmentResult.HasError())
	{
		return callingProcessEnvironmentResult;
	}

	Environment targetUserEnvironment;
	return GetEnvironmentWithSpecifiedByCaller(
		settings,
		Environment::Override(
			callingProcessEnvironmentResult.GetResultValue(),
			targetUserEnvironment,
			trace),
		trace);
}

Result<Environment> NewProcess::GetEnvironmentWithSpecifiedByCaller(const Settings& settings, const Environment& environment, Trace& trace) const
{
	return Environment::Apply(
		environment,
		Environment::CreateFormList(
			settings.GetEnvironmentVariables(),
			L"from command line",
			trace),
		trace);
}
