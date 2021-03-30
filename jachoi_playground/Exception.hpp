#pragma once
#include <string>
#include <iostream>
#include <exception>

class Exception : public std::exception
{
private:
	std::string msg;
public:
	Exception(const std::string& msg);
	virtual const char* what()const throw();
	virtual ~Exception() throw();
};
