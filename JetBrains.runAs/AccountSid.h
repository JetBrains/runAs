#pragma once
#include <string>

class AccountSid
{
	const wstring _sid;
	const wstring _name;
	const wstring _domain;

public:
	AccountSid(const wstring sid, const wstring name, const wstring domain);
	wstring GetSid() const;
	wstring GetName() const;
	wstring GetDomain() const;
	wstring ToString() const;
};

