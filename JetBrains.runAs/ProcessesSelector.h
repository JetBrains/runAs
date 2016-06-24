#pragma once
#include <queue>
#include "IProcess.h"
#include "ProcessAsUser.h"
#include "ProcessWithLogon.h"

class ProcessesSelector
{
	const ProcessAsUser _processAsUserElevated = ProcessAsUser(true);
	const ProcessAsUser _processAsUser = ProcessAsUser(false);
	const ProcessWithLogon _processWithLogonToChangeIntegrityLevel = ProcessWithLogon(true);
	const ProcessWithLogon _processWithLogon = ProcessWithLogon(false);

public:
	ProcessesSelector();
	const Result<queue<const IProcess*>> SelectProcesses(const Settings& settings) const;
};

