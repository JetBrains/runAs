#pragma once
#include "Result.h"
#include "CommanLineParser.h"
#include "Settings.h"
#include "SecurityManager.h"
#include "Handle.h"
#include "SelfTestStatistic.h"

class SecurityManager;
class Trace;

class SelfTest
{
	const SecurityManager _securityManager;

	Result<bool> IsService(Trace& trace, const Handle& token) const;
	Result<bool> HasAdministrativePrivileges(Trace& trace) const;
	Result<bool> HasPrivilege(Trace& trace, const Handle& token, const wstring privilege) const;
	Result<const IntegrityLevel> GetIntegrityLevel(Trace& trace, const Handle& token) const;
	static bool IsWow64();
	static bool Is64OS();	
public:
	SelfTest();
	Result<ExitCode> Run(const Settings& settings) const;
	Result<SelfTestStatistic> GetStatistic(const Settings& settings) const;
};

