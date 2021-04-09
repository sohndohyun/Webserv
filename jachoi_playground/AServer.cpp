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
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
#ifdef debug
#include <iostream>

using namespace std;
#endif
AServer::ServerException::ServerException(std::string const &msg) throw() : msg(msg){}
char const *AServer::ServerException::what() const throw()
{
	return const_cast<char*>(msg.c_str());
}

AServer::ServerException::~ServerException() throw() {}


AServer::AServer() {}
AServer::~AServer(){}

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
		if (listen(listenSocket, 3000) == -1)
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
			if (clients[i]->done)
				FD_SET(clients[i]->fd, &wset);
			else
				FD_SET(clients[i]->fd, &rset);
		}

		int selRet = select(fdMax + 1, &rset, &wset, NULL, NULL);
		if (selRet <= 0)
		{
			continue;
		}

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
				Client* cl = new Client(clntSocket);
				if (cl == NULL)
					continue;
				clients.push_back(cl);
				this->OnAccept(*cl);
			}
		}

		for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);

			if (FD_ISSET(cl->fd, &rset))
			{
				char buf[BUFSIZ];

				int str_len;
				str_len = recv(cl->fd, buf, BUFSIZ, 0);
				// if (str_len)
				// 	cout << str_len << endl;
				if (str_len == -1 && cl->request.empty())
				{
					OnDisconnect(*cl);
					FD_CLR(cl->fd, &rset);
					close(cl->fd);
					delete cl;
					it = clients.erase(it);
					continue;
				}
				if (str_len > 0)
					cl->request.append(buf, str_len);
				bool ischunk = cl->request.size() > 30 && std::string::npos != cl->request.find("\r\nTransfer-Encoding: chunked\r\n");
				if (cl->request.size() > 5 &&((ischunk && cl->request.substr(cl->request.size() - 5) == "0\r\n\r\n") ||
					(!ischunk && cl->request.substr(cl->request.size() - 4) == "\r\n\r\n")))
				{
					OnRecv(*cl);
				}
			}
			else if (FD_ISSET(cl->fd, &wset))
			{
				size_t sendsize = BUFSIZ;
				if (cl->sendCount * sendsize > cl->response.size())
					sendsize = cl->response.size() - (cl->sendCount - 1) * BUFSIZ;
				int ret = send(cl->fd, cl->response.c_str() + (cl->sendCount - 1) * BUFSIZ, sendsize, 0);
				cout << "sendsize : " << sendsize  <<  "   ret :  " << ret    << "    ressize : " << cl->response.size() << endl;
				if (ret <= sendsize)
				{
					cl->sendCount++;
				}
				else if (ret == -1)
				{
					cout << "========================\n";
					OnSend(*cl);
					cl->done = false;
					cl->sendCount = 1;
					cl->response.clear();
					cl->request.clear();
				}
				// if (ret <= 0)
				// {
				// 	OnDisconnect(*cl);
				// 	FD_CLR(cl->fd, &wset);
				// 	close(cl->fd);
				// 	delete cl;
				// 	it = clients.erase(it);
				// 	continue;
				// }
			}
			++it;
		}

		//Disconnect stub
		for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (cl->willDie && FD_ISSET(cl->fd, &rset) == 0 && cl->response.size() <= 0)
			// if (cl->willDie && !FD_ISSET(cl->fd, &rset) && cl->done)
			{
				OnDisconnect(*cl);
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

void AServer::disconnect(Client& cl)
{
	cl.willDie = true;
	// for (size_t i = 0;i < clients.size();i++)
	// {
	// 	if (clients[i]->fd == fd)
	// 	{
	// 		clients[i]->willDie = true;
	// 		break;
	// 	}
	// }
}

void AServer::sendStr(Client& cl, std::string const &str)
{
	if (str.size() == 0)
		return;

	// cout << "from sendstr" << endl;
	// cout << str << endl;

	cl.response.append(str);
	cl.done = true;
	// for (size_t i = 0;i < clients.size();i++)
	// {
	// 	if (clients[i]->fd == cl.fd)
	// 	{
	// 		clients[i]->send_buf.append(str);
	// 		clients[i]->done = true;
	// 		break;
	// 	}
	// }
}
