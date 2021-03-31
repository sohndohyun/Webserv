#ifndef webserver_hpp
#define webserver_hpp
#include "AServer.hpp"

class RequestParser;

class WebServer : public AServer
{
private:
	std::string reqStr;

public:
	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);

	void request_process(int fd, RequestParser const &req);
};

#endif
