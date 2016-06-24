#include "stdafx.h"
#include "SelfTestStatistic.h"

SelfTestStatistic::SelfTestStatistic()
	:SelfTestStatistic(false, false, false, INTEGRITY_LEVEL_AUTO)
{
}

SelfTestStatistic::SelfTestStatistic(
	const bool hasLogonSID, 
	const bool hasAdministrativePrivileges, 
	const bool hasSeAssignPrimaryTokenPrivilege,
	const IntegrityLevel integrityLevel)
	: _isService(hasLogonSID), _hasAdministrativePrivileges(hasAdministrativePrivileges), _hasSeAssignPrimaryTokenPrivilege(hasSeAssignPrimaryTokenPrivilege), _integrityLevel(integrityLevel)
{
}

bool SelfTestStatistic::IsService() const
{
	return _isService;
}

bool SelfTestStatistic::HasAdministrativePrivileges() const
{
	return _hasAdministrativePrivileges;
}

bool SelfTestStatistic::HasSeAssignPrimaryTokenPrivilege() const
{
	return _hasSeAssignPrimaryTokenPrivilege;
}

IntegrityLevel SelfTestStatistic::GetIntegrityLevel() const
{
	return _integrityLevel;
}
