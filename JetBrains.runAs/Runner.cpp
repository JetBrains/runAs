#include "stdafx.h"
#include "Runner.h"
#include "StreamWriter.h"
#include "Trace.h"
#include "Job.h"
#include "SelfTest.h"
#include "ProcessTracker.h"
#include <queue>
#include "ProcessesSelector.h"

Runner::Runner()
{		
}

Result<ExitCode> Runner::Run(const Settings& settings) const
{	
	if (settings.GetSelfTesting())
	{
		SelfTest selfTest;
		return selfTest.Run(settings);
	}
	
	return RunProcessAsUser(settings);
}

Result<ExitCode> Runner::RunProcessAsUser(const Settings& settings) const
{
	Trace trace(settings.GetLogLevel());	

	trace < L"Runner::Create a job";	
	Job job(false);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobObjectInfo = {};
	jobObjectInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	trace < L"Runner::Configure all child processes associated with the job to terminate when the parent is terminated";
	trace < L"Job::SetInformation";	
	job.SetInformation(JobObjectExtendedLimitInformation, jobObjectInfo);

	trace < L"Runner::Assign the current process to the job";
	trace < L"Job::AssignProcessToJob";
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	Handle currentProcess(L"Current process");
	currentProcess = GetCurrentProcess();
	job.AssignProcessToJob(currentProcess);

	const ProcessesSelector processesSelector;
	auto processesResult = processesSelector.SelectProcesses(settings);
	if(processesResult.HasError())
	{
		return processesResult.GetError();
	}

	auto processes = processesResult.GetResultValue();
	
	// Run process
	trace < L"::GetStdHandle(STD_OUTPUT_HANDLE)";
	StreamWriter stdOutput(GetStdHandle(STD_OUTPUT_HANDLE));
	trace < L"::GetStdHandle(STD_ERROR_HANDLE)";
	StreamWriter stdError(GetStdHandle(STD_ERROR_HANDLE));
	list<Result<ExitCode>> results;
	while(processes.size() > 0)
	{
		auto process = processes.front();		
		processes.pop();

		ProcessTracker processTracker(stdOutput, stdError);
		results.push_back(process->Run(settings, processTracker));
		if (results.back().HasError())
		{
			trace < L"Runner::Run failed";
			trace < L"Runner::Run error code: ";
			trace << results.back().GetError().GetCode();
			trace < L"Runner::Run error description: ";
			trace << results.back().GetError().GetDescription();
			continue;
		}

		if (results.back().GetResultValue() != STATUS_DLL_INIT_FAILED)
		{
			break;
		}
	}
	
	trace < L"Runner::Run finished";
	if (results.size() == 0)
	{
		return Result<ExitCode>(Error(L"RunProcessAsUser", 0, ERROR_CODE_UNKOWN, L"The processes are not available."));		
	}

	return results.back();	
}