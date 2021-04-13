#pragma once

#include <string>
#include <map>
#include <vector>

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

class Request
{
private:
	std::string leftStr;

	bool isHeaderMade;
	bool isChunk;
	bool isBodyMade;
	int bodysize;

public:
	std::map<std::string, std::string> header;
	std::string body;
	int errorCode;
	std::string path;
	std::string method;
	
public:
	Request();
	void init();
	void add(std::string const &str);
	bool valid() const;
	bool needRecv() const;
	MethodType methodType() const;


private:
	void parseFirstLine(std::string const &str);
	void parseHeader();
	void parseBody();
};