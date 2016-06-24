#pragma once
#include <string>
#include "ErrorCode.h"

class Error
{	
	const wstring _targetAction;
	const DWORD _win32Error;
	const ErrorCode _errorCode;
	const wstring _description;

	static ErrorCode GetErrorCodeInternal();
	static wstring GetLastErrorMessageInternal(const wstring& targetAction);	

public:		
	Error();
	explicit Error(const wstring& targetAction);
	Error(const wstring& targetAction, const wstring& arg);
	Error(const wstring& targetAction, const DWORD win32Error, const ErrorCode& errorCode, const wstring& description);

	wstring GetTarget() const;
	DWORD GetWin32Error() const;
	ErrorCode GetCode() const;
	wstring GetDescription() const;	
};

