#pragma once

#include "AServer.hpp"
#include "Request.hpp"
#include <map>

#ifdef __linux__
#define CGI_PATH "./ubuntu_cgi_tester"
#else
#define CGI_PATH "./cgi_tester"
#endif

#include "ConfigParse.hpp"
class Response;

class WebServer : public AServer
{
private:
	std::map<int, Request*> requests;

	std::map<int, std::string> reqStr;
	ConfigParse::t_conf &conf;

public:
	WebServer(ConfigParse::t_conf &conf);

	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);
	virtual ~WebServer();

	void request_process(int fd, Request &req);
	void cgi_stub(std::string const &path, Request &req, std::string &result);

	void methodGET(Response &res, Request &req);
	void methodHEAD(Response &res, Request &req);
	void methodPUT(Response &res, Request &req);
	void methodPOST(Response &res, Request &req);

private:
	void errorRes(Response &res, int errorCode, std::vector<std::string> allow_methods = std::vector<std::string>());
};
