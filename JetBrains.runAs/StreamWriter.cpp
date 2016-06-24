#include "stdafx.h"
#include "StreamWriter.h"

StreamWriter::StreamWriter(const HANDLE hStream)
	:_hStream(hStream)
{	
}

inline bool StreamWriter::WriteFile(const void* buffer, const unsigned long size)
{
	DWORD bytesWritten;
	return ::WriteFile(_hStream, buffer, size, &bytesWritten, nullptr) == TRUE;
}
