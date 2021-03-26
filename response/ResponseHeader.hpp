#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include <iostream>
# include <string>
# include <sys/time.h>
# include <time.h>
# include <map>
# include "ft_utils.hpp"
# include "../parser/RequestParser.hpp"
# include "../config/ConfigParse.hpp"

class ResponseHeader
{
public:
	ResponseHeader(int status_code, ConfigParse::t_server *conf_server, std::string content_type, std::map<std::string, std::string> req_header, std::string content_length);
	virtual ~ResponseHeader();

	class InvalidStatusCodeException: public std::exception {
		virtual const char *what() const throw();
	};

	std::map<std::string, std::string> header;
	std::map<std::string, std::string> status;

	void initStatus();

	void setStatus(int status_code);
	void setDate();
	void setServer(ConfigParse::t_server *conf_server);
	void setContentType(std::string content_type);
	void setContentLength(std::map<std::string, std::string> req_header, std::string content_length);
};


#endif
