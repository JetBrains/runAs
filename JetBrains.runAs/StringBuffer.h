#pragma once
#include <string>

class StringBuffer
{
	wchar_t* _buf;

public:
	StringBuffer(const wstring& str);
	~StringBuffer();
	wchar_t* GetPointer() const;
};

