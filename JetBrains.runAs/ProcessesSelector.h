#pragma once
#include <queue>
#include "IProcess.h"
#include "ProcessAsUser.h"
#include "ProcessWithLogon.h"
#include "NewProcess.h"

class ProcessesSelector
{
	const ProcessAsUser _processAsUserElevated = ProcessAsUser(true);
	const ProcessAsUser _processAsUser = ProcessAsUser(false);
	const ProcessWithLogon _processWithLogonToChangeIntegrityLevel = ProcessWithLogon(true);
	const ProcessWithLogon _processWithLogon = ProcessWithLogon(false);
	const NewProcess _newProcess = NewProcess();

public:
	ProcessesSelector();
	const Result<queue<const IProcess*>> SelectProcesses(const Settings& settings) const;
};

