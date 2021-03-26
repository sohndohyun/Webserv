#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include "ResponseHeader.hpp"

class Response
{
public:
	Response(std::string body);
	virtual ~Response();

	std::string body;
	std::string res_str;

	void makeRes(int status_code, ConfigParse::t_server *conf_server, std::string content_type, std::map<std::string, std::string> req_header, std::string content_length);
};

#endif
