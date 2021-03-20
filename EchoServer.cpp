#include "EchoServer.hpp"

void EchoServer::OnRecv(int fd, std::string const &str)
{
	this->sendStr(fd, str);
}

void EchoServer::OnSend(int fd)
{
	(void)&fd;
}

void EchoServer::OnAccept(int fd)
{
	(void)&fd;
}
