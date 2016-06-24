#include "stdafx.h"
#include "StringBuffer.h"
#include <iterator>


StringBuffer::StringBuffer(const wstring& str)
{
	auto size = str.size() + 1;
	_buf = new wchar_t[size];
	memcpy(_buf, str.c_str(), size * sizeof(_TCHAR));
}


StringBuffer::~StringBuffer()
{
	delete _buf;
}

wchar_t* StringBuffer::GetPointer() const
{
	return _buf;
}
