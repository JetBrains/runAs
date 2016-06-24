#pragma once
#include "ProcessAsUser.h"
#include "ProcessWithLogon.h"

class Job;

class Runner
{
	Result<ExitCode> RunProcessAsUser(const Settings& settings) const;

public:
	Runner();
	Result<ExitCode> Run(const Settings& settings) const;
};
