#pragma once
#include <string>
#include <iostream>
#include <exception>
#ifndef _NOEXCEPT
#define _NOEXCEPT throw()
#endif
class Exception : public std::exception
{
private:
	std::string msg;
public:
	Exception(const std::string& msg);
	virtual const char* what()const _NOEXCEPT;
	virtual ~Exception();
};
