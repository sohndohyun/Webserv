#ifndef echoserver_hpp
#define echoserver_hpp

#include "AServer.hpp"

class EchoServer : public AServer
{
public:
	virtual void OnRecv(Client& cl);
	virtual void OnSend(Client& cl);
	virtual void OnAccept(Client& cl);
	virtual void OnDisconnect(Client& cl);
};

#endif
