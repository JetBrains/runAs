#include "stdafx.h"
#include "SecurityManager.h"
#include "Handle.h"
#include "StringBuffer.h"
#include <memory>
#include "Trace.h"
#include "Sddl.h"
#include <set>
#include <sstream>

const list<wstring> AllPrivilegies = {
	SE_CREATE_TOKEN_NAME,
	SE_ASSIGNPRIMARYTOKEN_NAME,
	SE_LOCK_MEMORY_NAME,
	SE_INCREASE_QUOTA_NAME,
	SE_UNSOLICITED_INPUT_NAME,
	SE_MACHINE_ACCOUNT_NAME,
	SE_TCB_NAME,
	SE_SECURITY_NAME,
	SE_TAKE_OWNERSHIP_NAME,
	SE_LOAD_DRIVER_NAME,
	SE_SYSTEM_PROFILE_NAME,
	SE_SYSTEMTIME_NAME,
	SE_PROF_SINGLE_PROCESS_NAME,
	SE_INC_BASE_PRIORITY_NAME,
	SE_CREATE_PAGEFILE_NAME,
	SE_CREATE_PERMANENT_NAME,
	SE_BACKUP_NAME,
	SE_RESTORE_NAME,
	SE_SHUTDOWN_NAME,
	SE_DEBUG_NAME,
	SE_AUDIT_NAME,
	SE_SYSTEM_ENVIRONMENT_NAME,
	SE_CHANGE_NOTIFY_NAME,
	SE_REMOTE_SHUTDOWN_NAME,
	SE_UNDOCK_NAME,
	SE_SYNC_AGENT_NAME,
	SE_ENABLE_DELEGATION_NAME,
	SE_MANAGE_VOLUME_NAME,
	SE_IMPERSONATE_NAME,
	SE_CREATE_GLOBAL_NAME,
	SE_TRUSTED_CREDMAN_ACCESS_NAME,
	SE_RELABEL_NAME,
	SE_INC_WORKING_SET_NAME,
	SE_TIME_ZONE_NAME,
	SE_CREATE_SYMBOLIC_LINK_NAME
};

void SecurityManager::TrySetAllPrivileges(const Handle& token, const bool enablePrivileges)
{
	for (auto privilegiesIterrator = AllPrivilegies.begin(); privilegiesIterrator != AllPrivilegies.end(); ++privilegiesIterrator)
	{
		SetPrivileges(token, { *privilegiesIterrator }, true);
	}
}

Result<bool> SecurityManager::SetPrivileges(const Handle& token, const list<wstring>& privileges, const bool enablePrivileges)
{
	auto tokenPrivileges = static_cast<PTOKEN_PRIVILEGES>(_alloca(sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES) * (privileges.size() - 1)));
	tokenPrivileges->PrivilegeCount = static_cast<DWORD>(privileges.size());	
	auto index = 0;
	for (auto privilegesIterrator = privileges.begin(); privilegesIterrator != privileges.end(); ++privilegesIterrator)
	{		
		auto privilegeId = LookupPrivilegeValue(*privilegesIterrator);
		if (privilegeId.HasError())
		{
			return Result<bool>(privilegeId.GetError());
		}

		tokenPrivileges->Privileges[index].Luid = privilegeId.GetResultValue();
		tokenPrivileges->Privileges[index].Attributes = enablePrivileges ? SE_PRIVILEGE_ENABLED : 0;
		index++;
	}

	if (!AdjustTokenPrivileges(
		token,
		FALSE,
		tokenPrivileges,
		sizeof(TOKEN_PRIVILEGES),
		static_cast<PTOKEN_PRIVILEGES>(nullptr),
		static_cast<PDWORD>(nullptr)))
	{
		return Error(L"AdjustTokenPrivileges");
	}

	return true;
}

Result<LUID> SecurityManager::LookupPrivilegeValue(const wstring& privilegeName)
{
	StringBuffer privilegeNameBuf(privilegeName);
	LUID luid;
	if (!::LookupPrivilegeValue(
		nullptr,						// lookup privilege on local system
		privilegeNameBuf.GetPointer(),	// privilege to lookup 
		&luid))							// receives LUID of privilege
	{	
		return Error(L"LookupPrivilegeValue");
	}

	return luid;
}

Result<shared_ptr<void>> SecurityManager::GetTokenInformation(Trace& trace, const Handle& token, _TOKEN_INFORMATION_CLASS tokenInformationClass) const
{
	trace < L"SecurityManager::GetTokenInformation - Get the required buffer size.";
	LPVOID info = nullptr;
	DWORD length = 0;
	if (!::GetTokenInformation(token, tokenInformationClass, info, 0, &length))
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			return Error(L"GetTokenInformation");
		}
	}

	info = reinterpret_cast<PTOKEN_GROUPS>(new byte[length]);
	memset(info, 0, length);

	trace < L"SecurityManager::GetTokenInformation - Get the token information from the access token.";
	if (!::GetTokenInformation(token, tokenInformationClass, info, length, &length))
	{
		delete info;
		return Error(L"GetTokenInformation");
	}

	return shared_ptr<void>(info);
}

Result<list<AccountSid>> SecurityManager::GetTokenGroups(Trace& trace, const Handle& token) const
{
	#define MAX_NAME 1024
	SID_NAME_USE SidType;
	DWORD dwSize = MAX_NAME;
	TCHAR lpName[MAX_NAME];
	TCHAR lpDomain[MAX_NAME];

	auto tokenGroupsResult = GetTokenInformation(trace, token, TokenGroups);
	if (tokenGroupsResult.HasError())
	{
		return tokenGroupsResult.GetError();
	}

	list<AccountSid> result;
	auto groupsInfo = reinterpret_cast<PTOKEN_GROUPS>(tokenGroupsResult.GetResultValue().get());
	for (DWORD index = 0; index < groupsInfo->GroupCount; index++)
	{
		LPTSTR sidStr;
		if(!::ConvertSidToStringSid(groupsInfo->Groups[index].Sid, &sidStr))
		{
			continue;
		}

		if (!::LookupAccountSid(nullptr, groupsInfo->Groups[index].Sid,
			lpName, &dwSize, lpDomain,
			&dwSize, &SidType))
		{
			lpName[0] = 0;
			lpDomain[0] = 0;
		}

		result.push_back(AccountSid(sidStr, lpName, lpDomain));
	}

	return result;
}

Result<bool> SecurityManager::HasPrivilege(Trace& trace, const Handle& token, const wstring privilege) const
{
	auto privilegiesResult = GetPrivilegies(trace, token);
	if (privilegiesResult.HasError())
	{
		return Result<bool>(privilegiesResult.GetError());
	}

	auto privilegies = privilegiesResult.GetResultValue();
	return privilegies.find(privilege) != privilegies.end();
}

Result<set<wstring>> SecurityManager::GetPrivilegies(Trace& trace, const Handle& token) const
{
	auto tokenPrivilegiesResult = GetTokenInformation(trace, token, TokenPrivileges);
	if (tokenPrivilegiesResult.HasError())
	{
		return Result<set<wstring>>(tokenPrivilegiesResult.GetError());
	}

	wchar_t buf [256];
	WORD bufSize = 256 * sizeof(wchar_t);
	set<wstring> result;
	auto privilegiesInfo = reinterpret_cast<PTOKEN_PRIVILEGES>(tokenPrivilegiesResult.GetResultValue().get());
	for (DWORD index = 0; index < privilegiesInfo->PrivilegeCount; index++)
	{
		DWORD nameSize = bufSize;
		if (LookupPrivilegeNameW(nullptr, &privilegiesInfo->Privileges[index].Luid, buf, &nameSize))
		{
			result.insert(wstring(buf));
		}
	}

	return result;
}

Result<bool> SecurityManager::IsRunAsAdministrator() const
{
	auto isRunAsAdmin = FALSE;
	PSID pAdministratorsGroup = nullptr;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		return Error(L"AllocateAndInitializeSid");
	}

	if (!CheckTokenMembership(nullptr, pAdministratorsGroup, &isRunAsAdmin))
	{
		FreeSid(pAdministratorsGroup);
		return Error(L"CheckTokenMembership");
	}

	FreeSid(pAdministratorsGroup);
	return isRunAsAdmin == TRUE;
}

Result<bool> SecurityManager::SetIntegrityLevel(const IntegrityLevel& integrityLevelId, const Handle& securityToken, Trace& trace)
{
	if (integrityLevelId != INTEGRITY_LEVEL_AUTO)
	{
		trace < L"ProcessAsUser::Set integrity level to \"";
		trace << integrityLevelId;
		trace << L"\"";

		DWORD integrityLevel = SECURITY_MANDATORY_UNTRUSTED_RID;
		if (integrityLevelId == INTEGRITY_LEVEL_UNTRUSTED)
		{
			integrityLevel = SECURITY_MANDATORY_UNTRUSTED_RID;
		}

		if (integrityLevelId == INTEGRITY_LEVEL_LOW)
		{
			integrityLevel = SECURITY_MANDATORY_LOW_RID;
		}

		if (integrityLevelId == INTEGRITY_LEVEL_MEDIUM)
		{
			integrityLevel = SECURITY_MANDATORY_MEDIUM_RID;
		}

		if (integrityLevelId == INTEGRITY_LEVEL_MEDIUM_PLUS)
		{
			integrityLevel = SECURITY_MANDATORY_MEDIUM_PLUS_RID;
		}

		if (integrityLevelId == INTEGRITY_LEVEL_HIGH)
		{
			integrityLevel = SECURITY_MANDATORY_HIGH_RID;
		}

		if (integrityLevelId == INTEGRITY_LEVEL_SYSTEM)
		{
			integrityLevel = SECURITY_MANDATORY_SYSTEM_RID;
		}

		SID integrityLevelSid{};
		integrityLevelSid.Revision = SID_REVISION;
		integrityLevelSid.SubAuthorityCount = 1;
		integrityLevelSid.IdentifierAuthority.Value[5] = 16;
		integrityLevelSid.SubAuthority[0] = integrityLevel;

		TOKEN_MANDATORY_LABEL tokenIntegrityLevel = {};
		tokenIntegrityLevel.Label.Attributes = SE_GROUP_INTEGRITY;
		tokenIntegrityLevel.Label.Sid = &integrityLevelSid;

		trace < L"::SetTokenInformation";
		if (!SetTokenInformation(
			securityToken,
			TokenIntegrityLevel,
			&tokenIntegrityLevel,
			sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(&integrityLevelSid)))
		{
			return Error(L"SetTokenInformation");
		}

		return true;
	}

	return false;
}

Result<const IntegrityLevel> SecurityManager::GetIntegrityLevel(Trace& trace, const Handle& token) const
{
	SecurityManager securityManager;
	auto tokenInformationResult = securityManager.GetTokenInformation(trace, token, TokenIntegrityLevel);
	if (tokenInformationResult.HasError())
	{
		return tokenInformationResult.GetError();
	}

	auto pTIL = reinterpret_cast<PTOKEN_MANDATORY_LABEL>(tokenInformationResult.GetResultValue().get());
	auto dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, static_cast<DWORD>(static_cast<UCHAR>(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1)));
	switch (dwIntegrityLevel)
	{
	case SECURITY_MANDATORY_UNTRUSTED_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_UNTRUSTED);

	case SECURITY_MANDATORY_LOW_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_LOW);

	case SECURITY_MANDATORY_MEDIUM_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_MEDIUM);

	case SECURITY_MANDATORY_MEDIUM_PLUS_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_MEDIUM_PLUS);

	case SECURITY_MANDATORY_HIGH_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_HIGH);

	case SECURITY_MANDATORY_SYSTEM_RID:
		return Result<const IntegrityLevel>(INTEGRITY_LEVEL_SYSTEM);
	}

	return Result<const IntegrityLevel>(INTEGRITY_LEVEL_AUTO);
}

Result<bool> SecurityManager::HasGroupSid(Trace& trace, const Handle& token, wstring sid) const
{
	wstringstream text;
	auto tokenGroupsResult = GetTokenGroups(trace, token);
	if (tokenGroupsResult.HasError())
	{
		return tokenGroupsResult.GetError();
	}

	auto tokenGroups = tokenGroupsResult.GetResultValue();
	for (auto groupsIterrator = tokenGroups.begin(); groupsIterrator != tokenGroups.end(); ++groupsIterrator)
	{
		if (groupsIterrator->GetSid() == sid)
		{
			return true;
		}
	}

	return false;
}