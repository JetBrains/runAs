#pragma once
#include "Error.h"

template<typename T>
class Result
{
	bool _hasError;
	T _resultValue;
	Error _error;	

public:
	Result<T>();
	Result<T>(const T& resultValue);
	Result<T>(const Error& error);	

	bool HasError() const;
	T GetResultValue() const;
	Error GetError() const;	
};

template<typename T>
Result<T>::Result() : _hasError(false), _error(Error())
{
}

template<typename T>
Result<T>::Result(const T& resultValue)
	: _hasError(false), _resultValue(resultValue), _error(Error())
{	
}

template<typename T>
Result<T>::Result(const Error& error)
	:_hasError(true), _error(error)
{	 
}

template<typename T>
bool Result<T>::HasError() const
{
	return _hasError;
}

template<typename T>
T Result<T>::GetResultValue() const
{
	return _resultValue;
}

template<typename T>
Error Result<T>::GetError() const
{
	return _error;
}
