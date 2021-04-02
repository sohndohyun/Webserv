#pragma once
#include "PathParser.hpp"

enum MethodType
{
	GET,
	HEAD, 
	POST, 
	PUT, 
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	INVLAID = -1
};

class RequestParser
{
private:
	bool								_badreq;
	bool								_header_not_end;
	std::string							_http_version;
	bool								_http_version_specified;

public:
	std::string							method;
	int									errorcode;
	PathParser*							pathparser;
	std::map<std::string, std::string>	header;
	std::string							body;

public:
	RequestParser(const std::string& req);
	bool isBadRequest() const;
	int getMethodType() const;
	bool versionSpecified() const;
	virtual ~RequestParser();

	//dsohn added
	bool needRecvMore() const;
private:
	bool checkRequestValid(const std::string& reqhead);
};