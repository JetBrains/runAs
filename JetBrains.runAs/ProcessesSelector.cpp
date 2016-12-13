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

	queue<const IProcess*> processes;
	if(settings.GetUserName() == L"")
	{		
		trace < L"ProcessesSelector::SelectProcesses push Process";
		processes.push(&_newProcess);
		return processes;
	}

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
			
	if (statistic.HasAdministrativePrivileges())
	{
		// Has administrative privileges
		if (statistic.IsService())
		{
			// Is a service: for Local System services (INTEGRITY_LEVEL_SYSTEM) and services under admin (INTEGRITY_LEVEL_HIGH)
			auto elevationIsRequired = settings.GetIntegrityLevel() == INTEGRITY_LEVEL_SYSTEM || settings.GetIntegrityLevel() == INTEGRITY_LEVEL_HIGH;
			trace < L"ProcessesSelector::SelectProcesses elevationIsRequired = ";
			trace << elevationIsRequired;
			
			if (statistic.HasSeAssignPrimaryTokenPrivilege() && statistic.HasSeTcbPrivilegePrivilege())
			{
				// Has SeAssignPrimaryTokenPrivilege (Replace a process-level token)
				// Has SeTcbPrivilegePrivilege (Act as part of the operating system)
				trace < L"ProcessesSelector::SelectProcesses push ProcessAsUser";
				processes.push(&_processAsUser);
			}
		}
		else
		{
			// Is not a service
			auto changeIntegrityLevel = settings.GetIntegrityLevel() != INTEGRITY_LEVEL_DEFAULT;
			trace < L"ProcessesSelector::SelectProcesses changeIntegrityLevel = ";
			trace << changeIntegrityLevel;

			if (changeIntegrityLevel)
			{
				// Changing windows integrity level
				trace < L"ProcessesSelector::SelectProcesses push ProcessWithLogon to change Integrity Level";
				processes.push(&_processWithLogonToChangeIntegrityLevel);
			}
		}
	}

	// All other cases
	// Windows Integrity Level will be by default and can't be elevated
	trace < L"ProcessesSelector::SelectProcesses push ProcessWithLogon";
	processes.push(&_processWithLogon);
	return processes;
}
