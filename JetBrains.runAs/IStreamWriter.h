#pragma once
class IStreamWriter
{
public:
	virtual ~IStreamWriter() {};

	virtual bool WriteFile(const void* buffer, const unsigned long size) = 0;
};

