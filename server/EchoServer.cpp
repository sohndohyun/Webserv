#include "EchoServer.hpp"
#include <iostream>

void EchoServer::OnRecv(int fd, std::string const &str)
{
	this->sendStr(fd, str);
	std::cout << fd << ": " << str << "\n";
}

void EchoServer::OnSend(int fd)
{
	(void)&fd;
}

void EchoServer::OnAccept(int fd, int port)
{
	(void)&fd;
	(void)&port;
	std::cout << fd << "(" << port << "): accepted!" << "\n";
}

void EchoServer::OnDisconnect(int fd)
{
	(void)&fd;
	std::cout << fd << ": disconnected!" << "\n";
}
