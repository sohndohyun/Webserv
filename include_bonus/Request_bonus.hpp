#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "AServer_bonus.hpp"
#include <string>
#include <map>
#include <vector>

enum MethodType
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	INVLAID = -1
};

class Request
{
private:
	std::string leftStr;

	bool isHeaderMade;
	bool isChunk;
	bool isBodyMade;
	int bodysize;
	std::string _deserialize;
public:
	std::map<std::string, std::string> header;
	std::string body;
	int errorCode;
	std::string path;
	std::string method;
	std::string querystring;

public:
	Request();
	void init();
	void add(std::string const &str);
	bool valid() const;
	bool needRecv() const;
	MethodType methodType() const;

	void isAcceptLanguage(std::string &content_path, int is_dir);
	bool isAcceptCharset();
	void isReferer(AServer::t_analysis &analysis);
	void isUserAgent(AServer::t_analysis &analysis);
	std::string deserialize();
private:
	void parseFirstLine(std::string const &str);
	void parseHeader();
	void parseBody();
};
#endif
