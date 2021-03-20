#ifndef echoserver_hpp
#define echoserver_hpp

#include "AStatelessServer.hpp"

class EchoServer : public AStatelessServer
{
public:
	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd);
};

#endif