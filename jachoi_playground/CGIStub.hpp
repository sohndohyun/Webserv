#pragma once
#include <string>
#ifdef __linux__
#define DEFAULT_CGIPATH  "./ubuntu_cgi_tester"
#else
#define DEFAULT_CGIPATH "./cgi_tester"
#endif
class CGIStub
{
private:
	std::string result;
	std::string cgipath;
public:
	CGIStub(const std::string& path, const std::string& cgipath = DEFAULT_CGIPATH);
	const std::string getCGIResult();
	~CGIStub();
};
