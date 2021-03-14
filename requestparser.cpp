#include "requestparser.hpp"
#include "utils.hpp"
#include <iostream>

RequestParser::RequestParser(const std::string& req)
{
	size_t reqend = req.find("\r\n");
	std::string reqinfo = req.substr(0, reqend);
	_badreq = true;
	if (checkRequestValid(reqinfo))
		_badreq = false;
	//RequestHeaderParser
	//RequestBodyParser
}

bool RequestParser::isBadRequest() const
{
	return _badreq;
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
	_method = reqhead.substr(0, first);
	_http_version = reqhead.substr(last, reqhead.size());
	pathparser = PathParser(reqhead.substr(first + 1, last));
	errorcode = 0;
	if (std::find(_known_methods, _known_methods + 8, _method) == _known_methods + 8)
		errorcode = 400;
	if (_http_version.size() && (std::find(_supported_version, _supported_version + 2, _http_version) == _supported_version + 2))
		errorcode = 505;
	_http_version_specified = static_cast<bool>(_http_version.size());
	return errorcode != 400 && errorcode != 505;
}