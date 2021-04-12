// #pragma once

// #include <iostream>
// #include <string>
// #include <sys/time.h>
// #include <time.h>
// #include <map>
// #include "Utils.hpp"
// #include "RequestParser.hpp"
// #include "ConfigParse.hpp"

// class ResponseHeader
// {
// public:
// 	int statuscode;
// 	std::map<std::string, std::string> header;
// public:
// 	ResponseHeader(const std::string& servername, int statuscode, const std::string& body, bool chunked = false);

// 	std::string getStatus(int);
// 	void setDate();
// 	void setServer(ConfigParse::t_server *conf_server);
// 	void setContentType(const std::string&);
// 	void setContentLength(std::map<std::string, std::string> req_header, std::string content_length);
// };
