#include "stdafx.h"
#include "CaseInsensitiveComparer.h"

bool CaseInsensitiveComparer::operator()(const wstring& lhs, const wstring& rhs) const
{
	return _wcsicmp(lhs.c_str(), rhs.c_str()) < 0;
}
