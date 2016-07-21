#pragma once
#include "IProcess.h"
#include "Trace.h"
#include "Job.h"

class NewProcess : public IProcess
{
	Result<Environment> GetEnvironment(const Settings& settings, Trace& trace) const;
	Result<Environment> GetEnvironmentWithSpecifiedByCaller(const Settings& settings, const Environment& environment, Trace& trace) const;

public:
	Result<ExitCode> Run(const Settings& settings, ProcessTracker& processTracker) const override;	
};

