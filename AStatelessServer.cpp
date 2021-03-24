#include "AStatelessServer.hpp"
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include "ft_utils.hpp"

AStatelessServer::ServerException::ServerException(std::string const &msg) _NOEXCEPT : msg(msg){}
char const *AStatelessServer::ServerException::what() const _NOEXCEPT
{
	return const_cast<char*>(msg.c_str());
}

AStatelessServer::ServerException::~ServerException() _NOEXCEPT {}

AStatelessServer::Client::Client(int fd, std::string const &str) : fd(fd), str(str){}

AStatelessServer::AStatelessServer() {}
AStatelessServer::~AStatelessServer()
{

}

void AStatelessServer::run(std::string ip, std::vector<int> ports)
{
	if (ports.empty())
		throw AStatelessServer::ServerException("AStatelessServer: no port");
	in_addr_t inet_ip = inet_addr(ip.c_str());
	if (inet_ip == INADDR_NONE)
		throw AStatelessServer::ServerException("AStatelessServer: wrong ip");
	std::vector<int> listenSocks;
	int fdMax = -1;
	for (size_t i = 0;i < ports.size();i++)
	{
		int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket < 0)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			throw AStatelessServer::ServerException("AStatelessServer: socket error");
		}
		fcntl(listenSocket, F_SETFL, O_NONBLOCK);
		if (listenSocket > fdMax)
			fdMax = listenSocket;

		sockaddr_in servAddr;
		ft_memset(&servAddr, 0, sizeof(sockaddr_in));
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = ft_htons(ports[i]);
		servAddr.sin_addr.s_addr = inet_addr(ip.c_str());

		if (bind(listenSocket, (sockaddr*)&servAddr, sizeof(sockaddr_in)) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw AStatelessServer::ServerException("AStatelessServer: bind error");
		}
		if (listen(listenSocket, 5) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw AStatelessServer::ServerException("AStatelessServer: listen error");
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
			throw AStatelessServer::ServerException("AStatelessServer: select error");
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
		for (std::vector<AStatelessServer::Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (FD_ISSET(cl->fd, &rset))
			{
				char buf[BUFSIZ];
				int str_len = recv(cl->fd, buf, BUFSIZ, 0);
				if (str_len <= 0)
				{
					close(cl->fd);
					delete cl;
					it = clients.erase(it);
					continue;
				}
				std::string temp;
				temp.append(buf, str_len);
				this->OnRecv(cl->fd, temp);
			}
			else if (FD_ISSET(cl->fd, &wset))
			{
				int ret = send(cl->fd, cl->str.c_str(), cl->str.size(), 0);
				if (ret <= 0)
				{
					close(cl->fd);
					delete cl;
					it = clients.erase(it);
					continue;
				}
				if (ret < static_cast<int>(cl->str.size()))
					cl->str = cl->str.substr(ret);
				else
				{
					this->OnSend(cl->fd);
					close(cl->fd);
					delete cl;
					it = clients.erase(it);
					continue;
				}
			}
			++it;
		}
	}
	for (size_t i = 0;i < listenSocks.size();i++)
		close(listenSocks[i]);
}

void AStatelessServer::sendStr(int fd, std::string const &str)
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
