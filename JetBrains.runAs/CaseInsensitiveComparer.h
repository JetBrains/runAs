#pragma once
#include <string>

struct CaseInsensitiveComparer {
	bool operator()(const wstring& lhs, const wstring& rhs) const;
};
