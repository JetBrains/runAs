#include "stdafx.h"
#include "ProcessTracker.h"
#include <thread>
#include <chrono>
#include "Result.h"
#include "ExitCode.h"
#include "Trace.h"

ProcessTracker::ProcessTracker(IStreamWriter& outputWriter, IStreamWriter& errorWriter)
	: _outputWriter(outputWriter), _errorWriter(errorWriter)
{	
}

Result<bool> ProcessTracker::InitializeConsoleRedirection(SECURITY_ATTRIBUTES& securityAttributes, STARTUPINFO& startupInfo)
{
	auto errorStdOutPipe = _stdOutPipe.Initialize(securityAttributes);
	if (errorStdOutPipe.HasError() || !errorStdOutPipe.GetResultValue())
	{
		return errorStdOutPipe;
	}
	
	auto errorStdErrorOutPipe = _stdErrorOutPipe.Initialize(securityAttributes);
	if (errorStdErrorOutPipe.HasError() || !errorStdErrorOutPipe.GetResultValue())
	{
		return errorStdErrorOutPipe;
	}

	auto errorStdInPipe = _stdInPipe.Initialize(securityAttributes);
	if (errorStdInPipe.HasError() || !errorStdInPipe.GetResultValue())
	{
		return errorStdInPipe;
	}

	startupInfo.hStdOutput = _stdOutPipe.GetWriter();
	startupInfo.hStdError = _stdErrorOutPipe.GetWriter();
	startupInfo.hStdInput = _stdInPipe.GetReader();
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	return true;
}

Result<ExitCode> ProcessTracker::WaiteForExit(HANDLE processHandle, Trace& trace)
{
	trace < L"ProcessTracker::WaiteForExit";

	DWORD exitCode;
	bool hasData;
	do
	{
		this_thread::sleep_for(chrono::milliseconds(0));
		auto hasData1 = RedirectStream(_stdOutPipe.GetReader(), _outputWriter);
		if(hasData1.HasError())
		{
			return Result<ExitCode>(hasData1.GetError());
		}

		auto hasData2 = RedirectStream(_stdErrorOutPipe.GetReader(), _errorWriter);
		if (hasData2.HasError())
		{
			return Result<ExitCode>(hasData2.GetError());
		}

		hasData = hasData1.GetResultValue() | hasData2.GetResultValue();
		if (!GetExitCodeProcess(processHandle, &exitCode))
		{
			return Error(L"GetExitCodeProcess");
		}
	}
	while (exitCode == STILL_ACTIVE || hasData);

	trace < L"ProcessTracker::WaiteForExit finished";
	return exitCode;
}

Result<bool> ProcessTracker::RedirectStream(const HANDLE hPipeRead, IStreamWriter& writer)
{
	CHAR buffer[0xffff];
	DWORD bytesReaded;
	DWORD totalBytesAvail;
	DWORD bytesLeftThisMessage;

	if (!PeekNamedPipe(hPipeRead, buffer, sizeof(buffer), &bytesReaded, &totalBytesAvail, &bytesLeftThisMessage))
	{
		if (GetLastError() != ERROR_BROKEN_PIPE)
		{
			return Error(L"PeekNamedPipe");
		}
	}

	if (totalBytesAvail == 0)
	{
		return false;
	}

	if (!ReadFile(hPipeRead, buffer, bytesReaded, &bytesReaded, nullptr) || !bytesReaded)
	{
		if (GetLastError() == ERROR_BROKEN_PIPE)
		{
			// Pipe done - normal exit path.
			return false;
		}

		return Error(L"ReadFile");
	}

	if (!writer.WriteFile(buffer, bytesReaded))
	{
		return Error(L"WriteConsole");
	}

	return true;
}

