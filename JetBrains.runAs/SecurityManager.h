#pragma once
#include <list>
#include "Result.h"
#include "Trace.h"
#include <memory>
#include <set>
#include "AccountSid.h"
#include "IntegrityLevel.h"

class Handle;

class SecurityManager
{
	static Result<LUID> LookupPrivilegeValue(const wstring& privilegeName);
	
public:
	Result<bool> TrySetAllPrivileges(Trace& trace, const bool enablePrivileges) const;
	Result<bool> TrySetAllPrivileges(Trace& trace, const Handle& token, const bool enablePrivileges) const;
	Result<bool> SetPrivileges(
		Trace& trace,
		const list<wstring>& privileges,	// names of privileges to enable/disable
		const bool enablePrivileges			// to enable or disable privilege
	) const;
	Result<bool> SetPrivileges(
		Trace& trace,
		const Handle& token,				// access token handle
		const list<wstring>& privileges,	// names of privileges to enable/disable
		const bool enablePrivileges			// to enable or disable privilege
		) const;
	Result<shared_ptr<void>> GetTokenInformation(Trace& trace, const Handle& token, _TOKEN_INFORMATION_CLASS tokenInformationClass) const;
	Result<list<AccountSid>> GetTokenGroups(Trace& trace, const Handle& token) const;
	Result<bool> HasPrivilege(Trace& trace, const Handle& token, const wstring privilege) const;
	Result<set<wstring>> GetPrivilegies(Trace& trace, const Handle& token) const;
	Result<bool> IsRunAsAdministrator() const;
	Result<bool> SetIntegrityLevel(const IntegrityLevel& integrityLevel, const Handle& securityToken, Trace& trace) const;
	Result<const IntegrityLevel> GetIntegrityLevel(Trace& trace, const Handle& token) const;
	Result<bool> HasGroupSid(Trace& trace, const Handle& token, wstring sid) const;	
};

