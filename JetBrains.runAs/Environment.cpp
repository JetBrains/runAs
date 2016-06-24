#include "stdafx.h"
#include "Environment.h"
#include <regex>
#include "Handle.h"
#include "StringUtilities.h"
#include "Trace.h"
#include <set>

static const wregex EnvVarRegex = wregex(L"(.+)=(.*)");
static const wstring UserProfileEnvVarName = L"USERPROFILE";
static const wstring PathEnvVarName = L"PATH";
// In upper case
static const set<wstring> AutoOverrides = {
	L"USERDOMAIN",
	L"USERDOMAIN_ROAMINGPROFILE",
	L"USERNAME",
	L"USERPROFILE"
};

Result<Environment> Environment::CreateForCurrentProcess(Trace& trace)
{
	trace < L"Environment::CreateForCurrentProcess";
	auto environment = GetEnvironmentStringsW();
	Environment newEnvironment;
	try
	{
		newEnvironment.CreateVariableMap(environment, trace);
	}
	catch(...)
	{		
	}	

	FreeEnvironmentStringsW(environment);
	return newEnvironment;
}

Result<Environment> Environment::CreateForUser(const Handle& token, bool inherit, Trace& trace)
{
	trace < L"Environment::CreateForUser";
	Environment newEnvironment;
	LPVOID environment;
	if (!CreateEnvironmentBlock(&environment, token, inherit))
	{
		return Error(L"CreateEnvironmentBlock");
	}

	try
	{
		newEnvironment.CreateVariableMap(environment, trace);
	}
	catch (...)
	{
	}

	DestroyEnvironmentBlock(environment);
	return newEnvironment;
}

Environment Environment::CreateFormString(const wstring& variables, Trace& trace)
{
	trace < L"Environment::CreateFormString";
	return CreateFormList(StringUtilities::Split(variables, L"\n"), trace);
}

Environment Environment::CreateFormList(const list<wstring>& variables, Trace& trace)
{
	trace < L"Environment::CreateFormList";
	Environment environment;
	wsmatch matchResult;
	for (auto varsIterrator = variables.begin(); varsIterrator != variables.end(); ++varsIterrator)
	{
		if (!regex_search(*varsIterrator, matchResult, EnvVarRegex))
		{
			continue;
		}

		auto envName = StringUtilities::Convert(matchResult._At(1).str(), toupper);
		auto envValue = matchResult._At(2).str();
		environment._vars[envName] = envValue;
		environment._empty = false;
		TraceVarible(trace, envName, envValue);
	}

	return environment;
}

Environment Environment::Override(const Environment& baseEnvironment, const Environment& sourceEnvironment, Trace& trace)
{
	trace < L"Environment::Override";
	trace < L"Environment::Copy environment variables from base environment";
	Environment overridedEnvironment;
	for (auto varsIterator = baseEnvironment._vars.begin(); varsIterator != baseEnvironment._vars.end(); ++varsIterator)
	{
		overridedEnvironment._vars[varsIterator->first] = varsIterator->second;
		overridedEnvironment._empty = false;
		TraceVarible(trace, varsIterator->first, varsIterator->second);		
	}

	auto userProfileInLowCase = StringUtilities::Convert(overridedEnvironment._vars[UserProfileEnvVarName], tolower);

	trace < L"Environment::Override environment variables from source environment";
	for (auto varsIterator = sourceEnvironment._vars.begin(); varsIterator != sourceEnvironment._vars.end(); ++varsIterator)
	{
		auto name = varsIterator->first;
		auto value = varsIterator->second;
		auto overridedValue = overridedEnvironment._vars[name];
		auto valueInLowCase = StringUtilities::Convert(value, tolower);
		auto overridedValueInLowCase = StringUtilities::Convert(overridedValue, tolower);
		
		// Concat paths
		if (name == PathEnvVarName)
		{
			value = value + (overridedValue.length() > 0 && value.length() > 0 ? L";" : L"") + overridedValue;
			overridedEnvironment._vars[name] = value;
			TraceVarible(trace, name, value);
			continue;
		}
		
		// Override values when AutoOverrides all value containts a path to userProfile
		if (AutoOverrides.find(name) != AutoOverrides.end() || overridedValueInLowCase.find(userProfileInLowCase) != wstring::npos)
		{
			overridedEnvironment._vars[name] = value;
			TraceVarible(trace, name, value);
		}
	}

	return overridedEnvironment;
}

Environment Environment::Apply(const Environment& baseEnvironment, const Environment& sourceEnvironment, Trace& trace)
{
	trace < L"Environment::Apply";
	Environment newEnvironment;

	for (auto varsIterator = baseEnvironment._vars.begin(); varsIterator != baseEnvironment._vars.end(); ++varsIterator)
	{
		newEnvironment._vars[varsIterator->first] = varsIterator->second;
		newEnvironment._empty = false;
	}

	for (auto varsIterator = sourceEnvironment._vars.begin(); varsIterator != sourceEnvironment._vars.end(); ++varsIterator)
	{
		newEnvironment._vars[varsIterator->first] = varsIterator->second;
		newEnvironment._empty = false;
		TraceVarible(trace, varsIterator->first, varsIterator->second);
	}

	return newEnvironment;
}

Environment::~Environment()
{	
	for (auto environmentBlockIterator = _environmentBlocks.begin(); environmentBlockIterator != _environmentBlocks.end(); ++environmentBlockIterator)
	{		
		delete *environmentBlockIterator;
	}

	_environmentBlocks.clear();
}

void Environment::CreateVariableMap(LPVOID environment, Trace& trace)
{
	_vars.clear();

	auto curVar = static_cast<LPCWSTR>(environment);
	size_t len;
	do
	{
		wstring curVarValue(curVar);
		len = curVarValue.size();
		if (len == 0)
		{
			continue;
		}

		curVar += len + 1;
		wsmatch matchResult;
		if (!regex_search(curVarValue, matchResult, EnvVarRegex))
		{
			continue;
		}

		auto envName = StringUtilities::Convert(matchResult._At(1).str(), toupper);;
		auto envValue = matchResult._At(2).str();
		_vars[envName] = envValue;
		TraceVarible(trace, envName, envValue);
		_empty = false;
	} while (len > 0);	
}

LPVOID* Environment::CreateEnvironmentFromMap()
{
	size_t size = 0;
	for (auto varsIterator = _vars.begin(); varsIterator != _vars.end(); ++varsIterator)
	{		
		size += varsIterator->first.size() + varsIterator->second.size() + 2;
	}

	size++;

	auto environment = new _TCHAR[size];
	memset(environment, 0, sizeof(_TCHAR) * size);

	size_t pointer = 0;
	for (auto varsIterator = _vars.begin(); varsIterator != _vars.end(); ++varsIterator)
	{
		auto curSize = varsIterator->first.size();		
		memcpy(environment + pointer, varsIterator->first.c_str(), curSize * sizeof(_TCHAR));
		pointer += curSize;

		environment[pointer] = _TCHAR('=');
		pointer ++;

		curSize = varsIterator->second.size();
		memcpy(environment + pointer, varsIterator->second.c_str(), curSize * sizeof(_TCHAR));
		pointer += curSize;		

		pointer ++;
	}	

	return reinterpret_cast<LPVOID*>(environment);
}

LPVOID* Environment::CreateEnvironment()
{
	if(_empty)
	{
		return nullptr;
	}

	auto environment = CreateEnvironmentFromMap();
	_environmentBlocks.push_back(environment);
	return environment;
}

void Environment::TraceVarible(Trace& trace, const wstring& key, const wstring& value)
{
	(trace < L"SET \"") << key << L"=" << value << L"\"";
}