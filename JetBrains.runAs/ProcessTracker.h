#pragma once
#include "Pipe.h"
#include "ExitCode.h"
#include "IStreamWriter.h"

class Trace;
class IStreamWriter;

class ProcessTracker
{
	Pipe _stdOutPipe = Pipe(L"StdOut");
	Pipe _stdErrorOutPipe = Pipe(L"StdErrorOut");
	Pipe _stdInPipe = Pipe(L"StdIn");	
	IStreamWriter& _outputWriter;
	IStreamWriter& _errorWriter;
	
	static Result<bool> RedirectStream(const HANDLE hPipeRead, IStreamWriter& writer);

public:
	ProcessTracker(IStreamWriter& outputWriter, IStreamWriter& errorWriter);
	Result<bool> InitializeConsoleRedirection(SECURITY_ATTRIBUTES& securityAttributes, STARTUPINFO& startupInfo);
	Result<ExitCode> WaiteForExit(const HANDLE processHandle, Trace& trace);
};

