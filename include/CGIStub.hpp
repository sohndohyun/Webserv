#pragma once
#include <string>
#ifdef __linux__
#define DEFAULT_CGIPATH  "./ubuntu_cgi_tester"
#else
#define DEFAULT_CGIPATH "./cgi/cgi_tester"
#endif

#include "RequestParser.hpp"

class CGIStub
{
private:
	std::string result;
	std::string cgipath;
public:
	CGIStub(RequestParser const &req, std::string path, const std::string& cgipath = DEFAULT_CGIPATH);
	const std::string& getCGIResult();
	~CGIStub();
};
