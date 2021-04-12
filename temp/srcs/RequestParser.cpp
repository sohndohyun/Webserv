#include "RequestParser.hpp"
#include "Utils.hpp"
#include <iostream>
#include <algorithm>
#include <stdio.h>

RequestParser::RequestParser(const std::string& req)
{
	pathparser = NULL;
	_header_not_end = false;
	if (req.find("\r\n\r\n") == std::string::npos)
	{
		_header_not_end = true;
		return ;
	}

	size_t reqend = req.find("\r\n");
	std::string reqinfo = req.substr(0, reqend);

	_badreq = true;

	if (checkRequestValid(reqinfo))
		_badreq = false;

	std::string tmp;
	int last = reqend + 2;
	while (true)
	{
		int end = req.find("\r\n", last);
		tmp = req.substr(last, end - last);
		if (tmp.size())
		{
			size_t colon_pos = tmp.find(":");
			if (colon_pos == std::string::npos)
			{
				_badreq = true;
				break;
			}
			header[tmp.substr(0, colon_pos)] = jachoi::ltrim(tmp.substr(colon_pos + 1));
			last = end + 2;
		}
		else
		{
			if (req.substr(last, 2) == "\r\n")
				body = req.substr(last + 2);
			else
				_badreq = true;
			break;
		}
	};
}

bool RequestParser::isBadRequest() const
{
	return _badreq;
}

RequestParser::~RequestParser()
{
	if (pathparser)
		delete pathparser;
}

bool RequestParser::checkRequestValid(const std::string& reqhead)
{
	// parse methods
	const char* _known_methods[] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"
	};
	const char* _supported_version[] = {
		"HTTP/1.1", "HTTP/1.0"
	};
	size_t first = reqhead.find_first_of(' ');
	size_t last = reqhead.find_last_of(' ');
	method = reqhead.substr(0, first);
	_http_version = reqhead.substr(last + 1, reqhead.size());
	pathparser = new PathParser(reqhead.substr(first + 1, last - first -1));
	errorcode = 0;
	if (std::find(_known_methods, _known_methods + 8, method) == _known_methods + 8)
		errorcode = 400;
	if (_http_version.size() && (std::find(_supported_version, _supported_version + 2, _http_version) == _supported_version + 2))
		errorcode = 505;
	_http_version_specified = static_cast<bool>(_http_version.size());
	return errorcode != 400 && errorcode != 505;
}

int RequestParser::getMethodType() const
{
	const char* _known_methods[] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"
	};
	for (size_t i = 0 ; i < sizeof(*_known_methods) ; i++)
	{
		if (_known_methods[i] == method)
			return i;
	}
	return -1;
}

bool RequestParser::versionSpecified() const
{
	return _http_version_specified;
}

bool RequestParser::needRecvMore() const
{
	if (_header_not_end)
	{
		return true;
	}
	switch (getMethodType())
	{
		case POST: case PUT: case CONNECT: case TRACE:
		{
			std::map<std::string, std::string>::const_iterator hit;
			hit = header.find("Transfer-Encoding");
			if (hit != header.end() && hit->second.find("chunked") != std::string::npos)
			{
				if (body[body.length() - 5] != '0')
					return true;
				hit = header.find("Content-Length");
				if (hit != header.end() && std::stoi(hit->second) > static_cast<int>(body.size()))
					return true;
			}
			break;
		}
	}
	return false;
}
