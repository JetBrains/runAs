#pragma once
#include "Result.h"
#include <list>
#include "ExitCode.h"
#include "LogLevel.h"
class Settings;

class CommanLineParser
{		
	bool TryGetValue(const list<wstring>& values, const wstring& str, wstring* value) const;

public:
	Result<Settings> TryParse(const list<wstring>& args, ExitCode* exitCodeBase, LogLevel* logLevel) const;	
};