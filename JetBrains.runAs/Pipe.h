#pragma once
#include <string>
#include "Handle.h"
#include "Result.h"

class Pipe
{
	Handle _readHandle;
	Handle _writeHandle;
	wstring _name;

public:
	explicit Pipe(const wstring& name);
	Result<bool> Initialize(SECURITY_ATTRIBUTES& securityAttributes);
	Handle& GetReader();
	Handle& GetWriter();
};
