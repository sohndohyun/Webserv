#pragma once
#include "PathParser.hpp"
class RequestParser
{
private:
	bool		_badreq;
	std::string	_method;
	std::string	_http_version;
	bool		_http_version_specified;
public:
	int									errorcode;
	PathParser*							pathparser;
	std::map<std::string, std::string>	header;
	std::string							body;
public:
	RequestParser(const std::string& req);
	bool isBadRequest() const;
	const std::string& getMethod() const;
	bool versionSpecified() const;
	virtual ~RequestParser();
private:
	bool checkRequestValid(const std::string& reqhead);
};