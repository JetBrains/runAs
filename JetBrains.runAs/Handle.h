#pragma once
#include <string>
#include "Result.h"

class Handle
{
	wstring _name;
	HANDLE _handle = nullptr;	
	void Close();

public:
	Handle();
	explicit Handle(const wstring& name);	
	~Handle();	
	operator HANDLE() const;
	Handle& operator = (const HANDLE handle);
	Handle& operator = (const Handle handle);
	PHANDLE operator &();
	bool IsInvalid() const;
	Result<Handle> Duplicate(const Handle& sourceProcess, const Handle& targetProcess, DWORD desiredAccess, bool inheritHandle, DWORD options) const;
	HANDLE Detach();
};
