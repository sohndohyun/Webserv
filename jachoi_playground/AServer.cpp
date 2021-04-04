#include "AServer.hpp"
#include <algorithm>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include "Utils.hpp"


#define debug
#ifdef debug
#include <iostream>
#ifdef BUFSIZ
#undef BUFSIZ
#endif
#define BUFSIZ 32768
using namespace std;
#endif
AServer::ServerException::ServerException(std::string const &msg) throw() : msg(msg){}
char const *AServer::ServerException::what() const throw()
{
	return const_cast<char*>(msg.c_str());
}

AServer::ServerException::~ServerException() throw() {}

AServer::Client::Client(int fd, std::string const &str) : fd(fd), str(str){willDie = false;}

AServer::AServer() {}
AServer::~AServer()
{

}

void AServer::run(std::string ip, std::vector<int> ports)
{
	if (ports.empty())
		throw AServer::ServerException("AServer: no port");
	in_addr_t inet_ip = inet_addr(ip.c_str());
	if (inet_ip == INADDR_NONE)
		throw AServer::ServerException("AServer: wrong ip");
	std::vector<int> listenSocks;
	int fdMax = -1;
	for (size_t i = 0;i < ports.size();i++)
	{
		int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket < 0)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			throw AServer::ServerException("AServer: socket error");
		}
		fcntl(listenSocket, F_SETFL, O_NONBLOCK);
		if (listenSocket > fdMax)
			fdMax = listenSocket;

		sockaddr_in servAddr;
		jachoi::memset(&servAddr, 0, sizeof(sockaddr_in));
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = jachoi::htons(ports[i]);
		servAddr.sin_addr.s_addr = inet_addr(ip.c_str());
		int option = 1;
		setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		if (bind(listenSocket, (sockaddr*)&servAddr, sizeof(sockaddr_in)) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw AServer::ServerException("AServer: bind error");
		}
		if (listen(listenSocket, 5) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw AServer::ServerException("AServer: listen error");
		}

		listenSocks.push_back(listenSocket);
	}

	fd_set rset, wset;
	while (true)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		for (size_t i = 0;i < listenSocks.size();i++)
			FD_SET(listenSocks[i], &rset);
		for (size_t i = 0;i < clients.size();i++)
		{
			if (clients[i]->str.size() > 0)
				FD_SET(clients[i]->fd, &wset);
			else
				FD_SET(clients[i]->fd, &rset);
		}
		int selRet = select(fdMax + 1, &rset, &wset, NULL, NULL);
		if (selRet == -1)
			throw AServer::ServerException("AServer: select error");
		else if (selRet == 0)
			continue ;
		for (size_t i = 0;i < listenSocks.size();i++)
		{
			if (FD_ISSET(listenSocks[i], &rset))
			{
				sockaddr_in clntAddr;
				socklen_t addrsize = sizeof(sockaddr_in);
				int clntSocket = accept(listenSocks[i], (sockaddr*)&clntAddr, &addrsize);
				if (clntSocket == -1)
					continue ;	// TODO: log when accept failed
				fcntl(clntSocket, F_SETFL, O_NONBLOCK);
				if (fdMax < clntSocket)
					fdMax = clntSocket;

				Client* cl = new Client(clntSocket, "");
				if (cl == NULL)
					continue;
				clients.push_back(cl);
				this->OnAccept(clntSocket, ports[i]);
			}
		}
		
		for (std::vector<AServer::Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (FD_ISSET(cl->fd, &rset))
			{
				char buf[BUFSIZ];

				int str_len;
				std::string temp;
				while ((str_len = recv(cl->fd, buf, BUFSIZ, 0)) > 0)
				{
					temp.append(buf, str_len);
					cout << "recv: " << str_len << endl;
					usleep(10000);
				}

				// if (str_len <= 0)
				// {
				// 	OnDisconnect(cl->fd);
				// 	FD_CLR(cl->fd, &rset);
				// 	close(cl->fd);
				// 	delete cl;
				// 	it = clients.erase(it);
				// 	continue;
				// }
				this->OnRecv(cl->fd, temp);
			}
			else if (FD_ISSET(cl->fd, &wset))
			{
				int ret = send(cl->fd, cl->str.c_str(), cl->str.size(), 0);
				if (ret <= 0)
				{
					OnDisconnect(cl->fd);
					FD_CLR(cl->fd, &wset);
					close(cl->fd);
					delete cl;
					it = clients.erase(it);
					continue;
				}
				if (ret < static_cast<int>(cl->str.size()))
					cl->str = cl->str.substr(ret);
				else
				{
					cl->str.clear();
					this->OnSend(cl->fd);
				}
			}
			++it;
		}
		for (std::vector<AServer::Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (cl->willDie && FD_ISSET(cl->fd, &rset) == 0 && cl->str.size() <= 0)
			{
				OnDisconnect(cl->fd);
				close(cl->fd);
				delete cl;
				it = clients.erase(it);
				continue;
			}
			it++;
		}
	}
	for (size_t i = 0;i < listenSocks.size();i++)
		close(listenSocks[i]);
}

void AServer::disconnect(int fd)
{
	for (size_t i = 0;i < clients.size();i++)
	{
		if (clients[i]->fd == fd)
		{
			clients[i]->willDie = true;
			break;
		}
	}
}

void AServer::sendStr(int fd, std::string const &str)
{
	for (size_t i = 0;i < clients.size();i++)
	{
		if (clients[i]->fd == fd)
		{
			clients[i]->str.append(str);
			break;
		}
	}
}
