#include "stdafx.h"
#include "ProcessAsUser.h"
#include "Settings.h"
#include "ProcessTracker.h"
#include "Result.h"
#include "ExitCode.h"
#include "Environment.h"
#include "Trace.h"
#include "StringBuffer.h"
#include "ShowModeConverter.h"
#include "SecurityManager.h"
class Trace;
class ProcessTracker;

ProcessAsUser::ProcessAsUser(const bool elevated)
	:_elevated(elevated)
{
}

Result<ExitCode> ProcessAsUser::Run(const Settings& settings, ProcessTracker& processTracker) const
{
	Trace trace(settings.GetLogLevel());
	trace < L"ProcessAsUser::Attempt to log a user on to the local computer";
	StringBuffer userName(settings.GetUserName());
	StringBuffer domain(settings.GetDomain());
	StringBuffer password(settings.GetPassword());
	StringBuffer workingDirectory(settings.GetWorkingDirectory());
	StringBuffer commandLine(settings.GetCommandLine());

	trace < L"::LogonUser";
	auto newUserSecurityTokenHandle = Handle(L"New user security token");
	if (!LogonUser(
		userName.GetPointer(),
		domain.GetPointer(),
		password.GetPointer(),
		_elevated ? LOGON32_LOGON_NETWORK : LOGON32_LOGON_INTERACTIVE,
		LOGON32_PROVIDER_DEFAULT,
		&newUserSecurityTokenHandle))
	{
		return Error(L"LogonUser");
	}	
	
	trace < L"ProcessAsUser::InitializeConsoleRedirection a new security descriptor";
	trace < L"::InitializeSecurityDescriptor";
	SECURITY_DESCRIPTOR securityDescriptor = {};
	if (!InitializeSecurityDescriptor(
		&securityDescriptor,
		SECURITY_DESCRIPTOR_REVISION))
	{
		return Error(L"InitializeSecurityDescriptor");
	}

	trace < L"::SetSecurityDescriptorDacl";
	if (!SetSecurityDescriptorDacl(
		&securityDescriptor,
		true,
		nullptr,
		false))
	{
		return Error(L"SetSecurityDescriptorDacl");
	}

	trace < L"ProcessAsUser::Creates a new access primary token that duplicates new process's token";
	auto primaryNewUserSecurityTokenHandle = Handle(L"Primary new user security token");
	SECURITY_ATTRIBUTES processSecAttributes = {};
	processSecAttributes.lpSecurityDescriptor = &securityDescriptor;
	processSecAttributes.nLength = sizeof(SECURITY_DESCRIPTOR);
	processSecAttributes.bInheritHandle = true;
	trace < L"::DuplicateTokenEx";
	if (!DuplicateTokenEx(
		newUserSecurityTokenHandle,
		0, // MAXIMUM_ALLOWED
		&processSecAttributes,
		SecurityImpersonation,
		TokenPrimary,
		&primaryNewUserSecurityTokenHandle))
	{
		return Error(L"DuplicateTokenEx");
	}	

	SECURITY_ATTRIBUTES threadSecAttributes = {};
	threadSecAttributes.lpSecurityDescriptor = nullptr;
	threadSecAttributes.nLength = 0;
	threadSecAttributes.bInheritHandle = false;	
	STARTUPINFO startupInfo = {};	

	trace < L"ProcessTracker::InitializeConsoleRedirection";
	auto error = processTracker.InitializeConsoleRedirection(processSecAttributes, startupInfo);
	if(error.HasError())
	{
		return Result<ExitCode>(error.GetError());
	}

	trace < L"::LoadUserProfile";
	PROFILEINFO profileInfo = {};
	profileInfo.dwSize = sizeof(PROFILEINFO);
	profileInfo.lpUserName = userName.GetPointer();
	if (!LoadUserProfile(primaryNewUserSecurityTokenHandle, &profileInfo))
	{
		return Error(L"LoadUserProfile");
	}

	auto newProcessEnvironmentResult = GetEnvironment(settings, primaryNewUserSecurityTokenHandle, settings.GetInheritanceMode(), trace);
	if (newProcessEnvironmentResult.HasError())
	{
		UnloadUserProfile(primaryNewUserSecurityTokenHandle, profileInfo.hProfile);
		return Result<ExitCode>(newProcessEnvironmentResult.GetError());
	}

	auto setIntegrityLevelResult = SecurityManager::SetIntegrityLevel(settings.GetIntegrityLevel(), primaryNewUserSecurityTokenHandle, trace);
	if (setIntegrityLevelResult.HasError())
	{
		return Result<ExitCode>(setIntegrityLevelResult.GetError());
	}

	trace < L"ProcessAsUser::Create a new process and its primary thread. The new process runs in the security context of the user represented by the specified token.";
	PROCESS_INFORMATION processInformation = {};
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = ShowModeConverter::ToShowWindowFlag(settings.GetShowMode());
	auto cmdLine = settings.GetCommandLine();
	trace < L"::CreateProcessAsUser";	
	if (!CreateProcessAsUser(
		primaryNewUserSecurityTokenHandle,
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
		auto result = Error(L"CreateProcessAsUser");
		UnloadUserProfile(primaryNewUserSecurityTokenHandle, profileInfo.hProfile);
		return result;
	}

	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	auto processHandle = Handle(L"Service Process");
	processHandle = processInformation.hProcess;
	
	auto threadHandle = Handle(L"Thread");
	threadHandle = processInformation.hThread;

	auto exitCode = processTracker.WaiteForExit(processInformation.hProcess, trace);
	UnloadUserProfile(primaryNewUserSecurityTokenHandle, profileInfo.hProfile);	

	return exitCode;
}

Result<Environment> ProcessAsUser::GetEnvironment(const Settings& settings, Handle& userToken, const InheritanceMode inheritanceMode, Trace& trace)
{
	auto callingProcessEnvironmentResult = Environment::CreateForCurrentProcess(trace);
	if(callingProcessEnvironmentResult.HasError())
	{
		return callingProcessEnvironmentResult;
	}

	if (inheritanceMode == INHERITANCE_MODE_ON)
	{
		return GetEnvironmentWithSpecifiedByCaller(
			settings, 
			callingProcessEnvironmentResult.GetResultValue(), 
			trace);
	}

	// Get target user's environment
	auto targetUserEnvironmentResult = Environment::CreateForUser(userToken, false, trace);
	if (targetUserEnvironmentResult.HasError())
	{
		return targetUserEnvironmentResult;
	}

	if (inheritanceMode == INHERITANCE_MODE_OFF)
	{
		return GetEnvironmentWithSpecifiedByCaller(
			settings, 
			targetUserEnvironmentResult.GetResultValue(), 
			trace);
	}
	
	return GetEnvironmentWithSpecifiedByCaller(
		settings, 
		Environment::Override(
			callingProcessEnvironmentResult.GetResultValue(), 
			targetUserEnvironmentResult.GetResultValue(), 
			trace),
		trace);
}

Result<Environment> ProcessAsUser::GetEnvironmentWithSpecifiedByCaller(const Settings& settings, const Environment& environment, Trace& trace)
{
	return Environment::Apply(
		environment, 
		Environment::CreateFormList(
			settings.GetEnvironmentVariables(), 
			trace), 
		trace);
}