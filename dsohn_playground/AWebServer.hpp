#ifndef echoserver_hpp
#define echoserver_hpp

#include "AServer.hpp"
#include "RequestParser.hpp"

class EchoServer : public AServer
{
private:
	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);
public:
	virtual void OnRequest(RequestParser const &req) = 0;
};

#endif