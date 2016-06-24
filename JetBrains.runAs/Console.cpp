#include "stdafx.h"
#include "Console.h"
#include "StreamWriter.h"

static StreamWriter StdOutput = StreamWriter(GetStdHandle(STD_OUTPUT_HANDLE));

Console& Console::operator<<(const wstring& text)
{
	string atext(text.begin(), text.end());
	StdOutput.WriteFile(atext.c_str(), static_cast<unsigned long>(atext.size()) * sizeof(char));
	return *this;
}
