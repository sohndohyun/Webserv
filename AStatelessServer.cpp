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

void AStatelessServer::run(int port)
{
	int listenSocket, fdMax;
	fdMax = listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0)
		throw AStatelessServer::ServerException("AStatelessServer: socket error");
	fcntl(listenSocket, F_SETFL, O_NONBLOCK);
	
	sockaddr_in servAddr;
	ft_memset(&servAddr, 0, sizeof(sockaddr_in));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = ft_htons(port);
	servAddr.sin_addr.s_addr = ft_htonl(INADDR_ANY);

	if (bind(listenSocket, (sockaddr*)&servAddr, sizeof(sockaddr_in)) == -1)
	{
		close(listenSocket);
		throw AStatelessServer::ServerException("AStatelessServer: bind error");
	}
	if (listen(listenSocket, 5) == -1)
	{
		close(listenSocket);
		throw AStatelessServer::ServerException("AStatelessServer: listen error");
	}

	fd_set rset, wset;
	while (true)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listenSocket, &rset);
		for (unsigned long i = 0;i < _clients.size();i++)
		{
			if (_clients[i]->str.size() > 0)
				FD_SET(_clients[i]->fd, &wset);
			else
				FD_SET(_clients[i]->fd, &rset);
		}
		int selRet = select(fdMax + 1, &rset, &wset, NULL, NULL);
		if (selRet == -1)
			throw AStatelessServer::ServerException("AStatelessServer: select error");
		else if (selRet == 0)
			continue ;
		
		if (FD_ISSET(listenSocket, &rset))
		{
			sockaddr_in clntAddr;
			socklen_t addrsize = sizeof(sockaddr_in);
			int clntSocket = accept(listenSocket, (sockaddr*)&clntAddr, &addrsize);
			if (clntSocket == -1)
				continue ;	// TODO: log when accept failed
			fcntl(clntSocket, F_SETFL, O_NONBLOCK);
			if (fdMax < clntSocket)
				fdMax = clntSocket;

			Client* cl = new Client(clntSocket, "");
			if (cl == NULL)
				continue;
			_clients.push_back(cl);
			this->OnAccept(listenSocket);
		}
		for (std::vector<AStatelessServer::Client*>::iterator it = _clients.begin(); it != _clients.end();)
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
					it = _clients.erase(it);
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
					it = _clients.erase(it);
					continue;
				}
				if (ret < static_cast<int>(cl->str.size()))
					cl->str = cl->str.substr(ret);
				else
				{
					this->OnSend(cl->fd);
					close(cl->fd);
					delete cl;
					it = _clients.erase(it);
					continue;
				}
			}
			++it;
		}
	}
	close(listenSocket);
}

void AStatelessServer::sendStr(int fd, std::string const &str)
{
	for (unsigned long i = 0;i < _clients.size();i++)
		if (_clients[i]->fd == fd)
		{
			_clients[i]->str.append(str);
			break;
		}
}
