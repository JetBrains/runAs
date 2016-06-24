#pragma once
#include "LogLevel.h"
#include "Console.h"

class Trace
{
	const LogLevel _logLevel;	
	Console _console;

public:
	explicit Trace(const LogLevel& logLevel);
	void WritePrefix();
	Trace& operator < (const wstring& text);
	Trace& operator << (const wstring& text);
	Trace& operator < (const size_t num);
	Trace& operator << (const size_t num);	
	Trace& operator < (const signed int num);
	Trace& operator << (const signed int num);
};

