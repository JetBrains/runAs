#include "stdafx.h"
#include "ProcessesSelector.h"
#include <queue>
#include "IProcess.h"
#include "SelfTest.h"

ProcessesSelector::ProcessesSelector()
{
}

const Result<queue<const IProcess*>> ProcessesSelector::SelectProcesses(const Settings& settings) const
{
	Trace trace(settings.GetLogLevel());
	trace < L"ProcessesSelector::SelectProcesses";

	const SelfTest selfTest;
	auto selfTestStatisticResult = selfTest.GetStatistic(settings);
	if (selfTestStatisticResult.HasError())
	{
		return selfTestStatisticResult.GetError();
	}

	auto statistic = selfTestStatisticResult.GetResultValue();

	trace < L"ProcessesSelector::SelectProcesses settings.GetIntegrityLevel() = ";
	trace << settings.GetIntegrityLevel();

	trace < L"ProcessesSelector::SelectProcesses statistic.GetIntegrityLevel() = ";
	trace << statistic.GetIntegrityLevel();
		
	queue<const IProcess*> processes;	
	if (statistic.IsService())
	{
		auto elevationIsRequired = settings.GetIntegrityLevel() == INTEGRITY_LEVEL_SYSTEM || settings.GetIntegrityLevel() == INTEGRITY_LEVEL_HIGH;
		trace < L"ProcessesSelector::SelectProcesses elevationIsRequired = ";
		trace << elevationIsRequired;

		// For Local System services (INTEGRITY_LEVEL_SYSTEM) and services under admin (INTEGRITY_LEVEL_HIGH)
		// Has SeAssignPrimaryToken Privilege		
		// Has Administrative Privileges
		if (statistic.HasAdministrativePrivileges() && statistic.HasSeAssignPrimaryTokenPrivilege())
		{
			if (elevationIsRequired)
			{
				// Should be elevated admin, if arg "-il" is "auto" or "high" or "system"
				trace < L"ProcessesSelector::SelectProcesses push ProcessAsUser Elevated";
				processes.push(&_processAsUserElevated);
			}
			else
			{
				trace < L"ProcessesSelector::SelectProcesses push ProcessAsUser";
				processes.push(&_processAsUser);
			}					
		}
	}
	else
	{
		auto changeIntegrityLevel = settings.GetIntegrityLevel() != INTEGRITY_LEVEL_DEFAULT;
		trace < L"ProcessesSelector::SelectProcesses changeIntegrityLevel = ";
		trace << changeIntegrityLevel;

		// Not a Service
		// For interactive admin accounts
		// Will be noninteractive
		if (changeIntegrityLevel && statistic.HasAdministrativePrivileges())
		{
			trace < L"ProcessesSelector::SelectProcesses push ProcessWithLogon to change Integrity Level";
			processes.push(&_processWithLogonToChangeIntegrityLevel);
		}
	}

	// For interactive Windows accounts
	// Integrity Level by default
	// Will be interactive
	// Can't be elevated
	// All other cases
	trace < L"ProcessesSelector::SelectProcesses push ProcessWithLogon";
	processes.push(&_processWithLogon);
	return processes;
}
