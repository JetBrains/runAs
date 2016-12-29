#pragma once
#include <list>

class StringUtilities
{
public:
	static list<wstring> Split(const wstring &str, const wstring& separator);
	static wstring Convert(const wstring& str, int(*const converter)(int));
	static wstring Join(const list<wstring> strings, const wstring& separator);
	static wstring Replace(const wstring& strSource, const wstring& replaceFrom, const wstring& replaceTo);
};

