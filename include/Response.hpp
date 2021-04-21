#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

class Response
{
private:
	std::string initStatus(int status_code);
	void setDate();

public:
	std::map<std::string, std::string> header;
	std::string res_str;

public:
	Response(const std::string& server_name);
	virtual ~Response();

	void setStatus(int status_code);
	void setContentType(const std::string& content_path);
	void setContentLocation(const std::string& req_path);
	void setAllow(const std::vector<std::string>& allowMethods);
	void setLocation(const std::string& req_path);
	void setLastModified(const std::string& content_path);
	void setRetryAfter(void);
	void setWWWAuthenticate(void);


	void makeRes(std::string body, bool isPUT = false, bool chunked = false);
};

#endif
