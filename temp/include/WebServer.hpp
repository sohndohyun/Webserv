#pragma once

#include "AServer.hpp"
#include "Request.hpp"
#include <map>

class WebServer : public AServer
{
private:
	std::map<int, Request*> requests;

public:
	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);
	virtual ~WebServer();

	void request_process(int fd, Request &req);
	void cgi_stub(std::string const &path, Request &req, std::string &result);
};
