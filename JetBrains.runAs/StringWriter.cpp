#include "stdafx.h"
#include <sstream>
#include "StringWriter.h"

StringWriter::StringWriter(wstringstream& stream)
	: _stream(stream)
{
}

inline bool StringWriter::WriteFile(const void* buffer, const unsigned long size)
{
	_stream << wstring(static_cast<const wchar_t*>(buffer), size / sizeof(wchar_t));
	return true;
}
