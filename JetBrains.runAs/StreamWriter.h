#pragma once
#include "IStreamWriter.h"

class StreamWriter: public IStreamWriter
{
	const HANDLE _hStream;

public:
	explicit StreamWriter(const HANDLE hStream);	
	bool WriteFile(const void* buffer, const unsigned long size) override;
};

