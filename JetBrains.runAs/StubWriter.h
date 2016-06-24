#pragma once
#include "IStreamWriter.h"

class StubWriter : public IStreamWriter
{
public:
	bool WriteFile(const void* buffer, const unsigned long size) override;
};

