#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <sys/time.h>
# include <time.h>
# include <map>
# include "Utils.hpp"
# include "RequestParser.hpp"
# include "ConfigParse.hpp"
# include "Exception.hpp"

class Response
{
private:
	std::string initStatus(int status_code);
	void setDate();

public:
	std::map<std::string, std::string> header;
	std::string res_str;

public:
	Response(std::string server_name);
	virtual ~Response();

	void setStatus(int status_code);
	void setContentType(std::string content_type);
	void makeRes(std::string body, std::string trans_encoded);
};

#endif
