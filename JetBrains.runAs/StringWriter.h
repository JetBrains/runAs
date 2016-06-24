#pragma once
#include "IStreamWriter.h"

class StringWriter: public IStreamWriter
{
	wstringstream& _stream;

public:
	explicit StringWriter(wstringstream& stream);
	bool WriteFile(const void* buffer, const unsigned long size) override;
};