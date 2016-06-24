#include "stdafx.h"
#include "Trace.h"
#include "Console.h"

Trace::Trace(const LogLevel& logLevel)
	:_logLevel(logLevel)
{	
}

Trace& Trace::operator<(const wstring& text)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		WritePrefix();
		_console << text;
	}

	return *this;
}

Trace& Trace::operator<<(const wstring& text)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		_console << text;
	}

	return *this;
}

Trace& Trace::operator<(const size_t num)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		WritePrefix();
		_console << to_wstring(num);
	}

	return *this;
}

Trace& Trace::operator<(const signed int num)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		WritePrefix();
		_console << to_wstring(num);
	}

	return *this;
}

Trace& Trace::operator<<(const size_t num)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		_console << to_wstring(num);
	}

	return *this;
}

Trace& Trace::operator<<(const signed int num)
{
	if (_logLevel == LOG_LEVEL_DEBUG)
	{
		_console << to_wstring(num);
	}

	return *this;
}

void Trace::WritePrefix()
{
	_console << L"\r\n";
	_console << L"DEBUG: ";
}