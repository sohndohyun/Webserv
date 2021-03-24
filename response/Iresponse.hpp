#ifndef IRESPONSE_HPP
# define IRESPONSE_HPP

# include <iostream>
# include <map>
# include "../config/ConfigParse.hpp"

class Iresponse
{
public:
	virtual ~Iresponse() {};
	virtual void setDate() = 0;
	virtual void setServer(ConfigParse::t_server *conf_server) = 0;
	virtual void setContentType(std::string content_type) = 0;
	virtual void setContentLength(std::string content_length) = 0;

	std::map<std::string, std::string> header;
};


#endif
