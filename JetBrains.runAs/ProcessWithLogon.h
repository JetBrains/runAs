#pragma once
#include "IProcess.h"
#include "ExitCode.h"
#include "Result.h"
class Trace;
class Settings;

class ProcessWithLogon : public IProcess
{
	const bool _changeIntegrityLevel;
	Result<ExitCode> RunInternal(Trace& trace, const Settings& settings, ProcessTracker& processTracker, Environment& environment) const;

public:
	explicit ProcessWithLogon(const bool elevated);
	Result<ExitCode> Run(const Settings& settings, ProcessTracker& processTracker) const override;	
};
