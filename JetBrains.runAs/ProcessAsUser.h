#pragma once
#include "IProcess.h"
#include "Settings.h"
#include "ExitCode.h"
#include "Result.h"
#include "Handle.h"
class Trace;
class Settings;

class ProcessAsUser: public IProcess
{		
	static Result<Environment> GetEnvironment(const Settings& settings, Handle& userToken, InheritanceMode inheritanceMode, Trace& trace);
	static Result<Environment> GetEnvironmentWithSpecifiedByCaller(const Settings& settings, const Environment& environment, Trace& trace);
	const bool _elevated;

public:
	explicit ProcessAsUser(const bool elevated);
	Result<ExitCode> Run(const Settings& settings, ProcessTracker& processTracker) const override;
};