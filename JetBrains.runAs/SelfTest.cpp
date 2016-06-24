#include "stdafx.h"
#include "SelfTest.h"
#include "Trace.h"
#include "ExitCode.h"
#include "Settings.h"
#include "SecurityManager.h"
#include "SelfTestStatistic.h"

class IntegrityLevelManager;

SelfTest::SelfTest()
	:_securityManager(SecurityManager())
{	
}

Result<ExitCode> SelfTest::Run(const Settings& settings) const
{	
	auto statisticResult = GetStatistic(settings);
	if (statisticResult.HasError())
	{
		return statisticResult.GetError();
	}

	auto statistic = statisticResult.GetResultValue();		
	if (statistic.IsService())
	{	
		if (!statistic.HasAdministrativePrivileges())
		{
			return EXIT_CODE_NO_ADMIN;
		}

		if (!statistic.HasSeAssignPrimaryTokenPrivilege())
		{
			return EXIT_CODE_NO_ASSIGN_PRIMARY_TOKEN_PRIV;
		}
	}
	
	return Is64OS() ? 64 : 32;
}

Result<SelfTestStatistic> SelfTest::GetStatistic(const Settings& settings) const
{
	Trace trace(settings.GetLogLevel());
	trace < L"SelfTest::GetStatistic";

	Handle processToken(L"Process token");
	if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &processToken))
	{
		return Error(L"OpenProcessToken");
	}

	trace < L"Token groups:";
	auto tokenGroupsResult = _securityManager.GetTokenGroups(trace, processToken);
	if (!tokenGroupsResult.HasError())
	{
		auto tokenGroups = tokenGroupsResult.GetResultValue();
		for (auto groupsIterrator = tokenGroups.begin(); groupsIterrator != tokenGroups.end(); ++groupsIterrator)
		{
			trace < groupsIterrator->ToString();
		}
	}	
	
	auto isServiceResult = IsService(trace, processToken);
	if (isServiceResult.HasError())
	{
		return isServiceResult.GetError();
	}

	trace < L"SelfTest::IsService: ";
	trace << isServiceResult.GetResultValue();

	auto hasAdministrativePrivilegesResult = HasAdministrativePrivileges(trace);
	if (hasAdministrativePrivilegesResult.HasError())
	{
		return hasAdministrativePrivilegesResult.GetError();
	}

	trace < L"SelfTest::HasAdministrativePrivileges: ";
	trace << hasAdministrativePrivilegesResult.GetResultValue();

	auto hasSeAssignPrimaryTokenPrivilegeResult = HasPrivilege(trace, processToken, SE_ASSIGNPRIMARYTOKEN_NAME);
	if (hasSeAssignPrimaryTokenPrivilegeResult.HasError())
	{
		return hasSeAssignPrimaryTokenPrivilegeResult.GetError();
	}

	trace < L"SelfTest::HasPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME): ";
	trace << hasSeAssignPrimaryTokenPrivilegeResult.GetResultValue();

	auto integrityLevelResult = GetIntegrityLevel(trace, processToken);
	if (integrityLevelResult.HasError())
	{
		return integrityLevelResult.GetError();
	}
	
	
	trace < L"SelfTest::GetIntegrityLevel: ";
	trace << integrityLevelResult.GetResultValue();

	return SelfTestStatistic(
		isServiceResult.GetResultValue(),
		hasAdministrativePrivilegesResult.GetResultValue(),
		hasSeAssignPrimaryTokenPrivilegeResult.GetResultValue(),
		integrityLevelResult.GetResultValue());
}

Result<bool> SelfTest::IsService(Trace& trace, const Handle& token) const
{		
	auto integrityLevelResult = _securityManager.GetIntegrityLevel(trace, token);
	if(integrityLevelResult.GetResultValue() == INTEGRITY_LEVEL_SYSTEM)
	{
		return true;
	}

	return _securityManager.HasGroupSid(trace, token, L"S-1-5-6");
}

Result<bool> SelfTest::HasAdministrativePrivileges(Trace& trace) const
{
	return _securityManager.IsRunAsAdministrator();
}

Result<bool> SelfTest::HasPrivilege(Trace& trace, const Handle& token, const wstring privilege) const
{
	return _securityManager.HasPrivilege(trace, token, privilege);
}

Result<const IntegrityLevel> SelfTest::GetIntegrityLevel(Trace& trace, const Handle& token) const
{
	return _securityManager.GetIntegrityLevel(trace, token);
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
bool SelfTest::IsWow64()
{
	int bIsWow64 = false;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process"));
	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			return false;
		}
	}

	return bIsWow64 != 0;
}

bool SelfTest::Is64OS()
{
#if defined(_M_X64) || defined(x86_64)
	return true;
#else
	return IsWow64() == true;
#endif
}
