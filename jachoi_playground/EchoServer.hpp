#ifndef echoserver_hpp
#define echoserver_hpp

#include "AServer.hpp"

class EchoServer : public AServer
{
public:
	virtual bool OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);
};

#endif
