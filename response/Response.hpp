#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <sys/time.h>
# include <time.h>
# include "Iresponse.hpp"
# include "ft_utils.hpp"

class Response: public Iresponse
{
public:
	Response(std::string http_v, std::string status_code, std::string status_msg, ConfigParse::t_server *conf_server, std::string content_type, std::string content_length);
	virtual ~Response();

	void setDate();
	void setServer(ConfigParse::t_server *conf_server);
	void setContentType(std::string content_type);
	void setContentLength(std::string content_length);
};


#endif
