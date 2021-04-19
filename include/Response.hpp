#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>

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
	void setContentType(std::string content_path);
	void setContentLocation(std::string req_path);
	void setAllow(std::vector<std::string> allowMethods);
	void setLocation(std::string req_path);
	void setLastModified(std::string content_path);
	void setRetryAfter(void);
	void setWWWAuthenticate(void);


	void makeRes(std::string body, bool isPUT = false, bool chunked = false);
};

#endif
