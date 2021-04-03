#pragma once
#include <string>
#define DEFAULT_CGIPATH  "./ubuntu_cgi_tester"
class CGIStub
{
private:
	std::string result;
	std::string cgipath;
public:
	CGIStub(const std::string& path, const std::string& cgipath = DEFAULT_CGIPATH);
	const std::string& getCGIResult();
	~CGIStub();
};
