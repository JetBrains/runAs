#include "stdafx.h"
#include "StringUtilities.h"
#include <algorithm>
#include <list>
#include <sstream>

list<wstring> StringUtilities::Split(const wstring& str, const wstring& separator)
{
	list<wstring> strs;
	auto separatorSize = separator.size();
	size_t start = 0;
	size_t pos;
	while ((pos = str.find(separator, start)) != wstring::npos)
	{
		strs.push_back(str.substr(start, pos - start - separatorSize));
		start = pos + separatorSize;		
	}

	pos = str.size() - separatorSize;
	if (start < pos)
	{
		strs.push_back(str.substr(start, pos));
	}

	return strs;
}

wstring StringUtilities::Convert(const wstring& str, int(*const converter)(int))
{
	wstring convertedStr;
	convertedStr.resize(str.size());
	transform(str.begin(), str.end(), convertedStr.begin(), converter);
	return convertedStr;
}

wstring StringUtilities::Join(const list<wstring> strings, const wstring& separator)
{
	wstringstream result;
	auto first = true;
	for (auto stringIterrator = strings.begin(); stringIterrator != strings.end(); ++stringIterrator)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			result << separator;
		}
		
		result << *stringIterrator;		
	}

	return result.str();
}