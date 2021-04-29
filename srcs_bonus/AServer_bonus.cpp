#include "AServer_bonus.hpp"
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include "Utils_bonus.hpp"
#include "Exception_bonus.hpp"
#include <iostream>
#include <errno.h>
#include <cstring>
#include <pthread.h>
#include "Request.hpp"

size_t workThreadNo = 0;
pthread_mutex_t workno_mutex;
pthread_mutex_t update_mutex;

AServer::Client::Client(int fd, std::string const &str, int port) : fd(fd), port(port), willDie(false), str(str) {}
AServer::Command::Command(CMDType type, int fd, int port, std::string const &str, void *temp) : type(type), fd(fd), port(port), str(str), temp(temp){}

void *AServer::AcceptThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);
	timeval tval;
	tval.tv_usec = 10;
	tval.tv_sec = 0;
	while (true)
	{
		int fdMax = -1;
		fd_set set;
		FD_ZERO(&set);
		for (size_t i = 0;i < server->listenSocks.size();i++)
		{
			if (fdMax < server->listenSocks[i])
				fdMax = server->listenSocks[i];
			FD_SET(server->listenSocks[i], &set);
		}
		int selRet = select(fdMax + 1, &set, NULL, NULL, &tval);
		if (selRet == -1)
		{
			std::cerr << "AcceptThread: select error\n";
			break ;
		}
		else if (selRet == 0)
			continue ;

		for (size_t i = 0;i < server->listenSocks.size();i++)
		{
			if (FD_ISSET(server->listenSocks[i], &set))
			{
				sockaddr_in clntAddr;
				socklen_t addrsize = sizeof(sockaddr_in);
				int clntSocket = accept(server->listenSocks[i], (sockaddr*)&clntAddr, &addrsize);
				if (clntSocket == -1)
					continue ;
				fcntl(clntSocket, F_SETFL, O_NONBLOCK);
				if (fdMax < clntSocket)
					fdMax = clntSocket;

				server->pushCommand(new Command(ACCEPT, clntSocket, server->ports[i], "", NULL));
			}
		}
	}
	return arg;
}

void* AServer::WorkerThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);
	int fdMax = 3;
	size_t threadNo;

	pthread_mutex_lock(&workno_mutex);
	threadNo = workThreadNo++;
	pthread_mutex_unlock(&workno_mutex);

	std::vector<Client*> &clients = server->clients[threadNo];

	fd_set wset, rset;
	timeval tval;
	tval.tv_usec = 10;
	tval.tv_sec = 0;
	while (true)
	{
		FD_ZERO(&wset);
		FD_ZERO(&rset);

		pthread_mutex_lock(&server->clientMutexs[threadNo]);
		for (size_t i = 0;i < clients.size();i++)
		{
			if (clients[i]->str.size() > 0)
				FD_SET(clients[i]->fd, &wset);
			else
				FD_SET(clients[i]->fd, &rset);
			if (fdMax < clients[i]->fd)
				fdMax = clients[i]->fd;
		}
		pthread_mutex_unlock(&server->clientMutexs[threadNo]);

		int selRet = select(fdMax + 1, &rset, &wset, NULL, &tval);
		if (selRet == -1)
		{
			std::cerr << "WorkerThread: select error\n";
			break ;
		}
		else if (selRet == 0)
			continue ;

		pthread_mutex_lock(&server->clientMutexs[threadNo]);
		for (std::vector<AServer::Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (FD_ISSET(cl->fd, &rset))
			{
				char buf[BUFSIZ];
				int str_len = recv(cl->fd, buf, BUFSIZ, 0);
				if (str_len == 0)
				{
					it = clients.erase(it);
					FD_CLR(cl->fd, &rset);
					server->pushCommand(new Command(DISCONNECT, cl->fd, cl->port, "", NULL));
					delete cl;
					continue;
				}
				else if (str_len < 0)
					continue;
				std::string temp;
				temp.append(buf, str_len);
				server->pushCommand(new Command(RECV, cl->fd, cl->port, temp, NULL));
			}
			else if (FD_ISSET(cl->fd, &wset))
			{
				int ret = send(cl->fd, cl->str.c_str(), cl->str.size(), 0);
				if (ret == 0)
				{
					it = clients.erase(it);
					FD_CLR(cl->fd, &wset);
					server->pushCommand(new Command(DISCONNECT, cl->fd, cl->port, "", NULL));
					delete cl;
					continue;
				}
				else if (ret < 0)
					continue;
				if (ret < static_cast<int>(cl->str.size()))
					cl->str = cl->str.substr(ret);
				else
				{
					cl->str.clear();
					server->pushCommand(new Command(SEND, cl->fd, cl->port, "", NULL));
				}
			}
			++it;
		}
		for (std::vector<AServer::Client*>::iterator it = clients.begin(); it != clients.end();)
		{
			Client *cl = (*it);
			if (cl->willDie && FD_ISSET(cl->fd, &rset) == 0 && cl->str.size() <= 0)
			{
				it = clients.erase(it);
				server->pushCommand(new Command(DISCONNECT, cl->fd, cl->port, "", NULL));
				delete cl;
				continue;
			}
			it++;
		}
		pthread_mutex_unlock(&server->clientMutexs[threadNo]);
	}
	return arg;
}

bool AServer::fileProcess()
{
	int fdMax = 3;
	fd_set wset, rset;
	timeval tval;
	tval.tv_usec = 0;
	tval.tv_sec = 0;

	FD_ZERO(&wset);
	FD_ZERO(&rset);

	for (size_t i = 0;i < writeFiles.size();i++)
	{
		if (fdMax < writeFiles[i]->fd)
			fdMax = writeFiles[i]->fd;
		FD_SET(writeFiles[i]->fd, &wset);
	}		
	for (size_t i = 0;i < readFiles.size();i++)
	{
		if (fdMax < readFiles[i]->fd)
			fdMax = readFiles[i]->fd;
		FD_SET(readFiles[i]->fd, &rset);
	}

	int selRet = select(fdMax + 1, &rset, &wset, NULL, &tval);
	if (selRet == -1)
	{
		std::cerr << "FileThread: " << strerror(errno) << "\n";
		return false;
	}
	else if (selRet == 0)
		return false;
	
	for (std::vector<Command*>::iterator it = readFiles.begin(); it != readFiles.end();)
	{
		Command *wf = *it;
		if (FD_ISSET(wf->fd, &rset))
		{
			char buf[BUFSIZ];
			int str_len = read(wf->fd, buf, BUFSIZ);
			if (str_len < 0)
			{
				it = readFiles.erase(it);
				delete wf;
				return true;
			}
			if (str_len == 0)
			{
				it = readFiles.erase(it);
				OnFileRead(wf->fd, wf->str, wf->temp);
				delete wf;
				return true;
			}
			wf->str.append(buf, str_len);
		}
		it++;
	}
	for (std::vector<Command*>::iterator it = writeFiles.begin(); it != writeFiles.end();)
	{
		Command *wf = *it;
		if (FD_ISSET(wf->fd, &wset))
		{
			int ret = write(wf->fd, wf->str.c_str(), wf->str.size());
			if (ret <= 0)
			{
				it = writeFiles.erase(it);
				delete wf;
			}
			else
			{
				it = writeFiles.erase(it);
				OnFileWrite(wf->fd, wf->temp);
				delete wf;
			}
			break;
		}
	}
	return true;
}

void *AServer::UpdateThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);
	Command *cmd;

	while (true)
	{
		if (server->fileProcess())
			continue;
		pthread_mutex_lock(&update_mutex);
		if (!server->commands.empty())
		{
			cmd = server->commands.front();
			server->commands.pop();
		}
		else
		{
			pthread_mutex_unlock(&update_mutex);
			continue;
		}
		pthread_mutex_unlock(&update_mutex);

		switch(cmd->type)
		{
		case RECV:
			server->OnRecv(cmd->fd, cmd->port, cmd->str);
			break;
		case SEND:
			server->OnSend(cmd->fd, cmd->port);
			break;
		case ACCEPT:
			server->OnAccept(cmd->fd, cmd->port);
			pthread_mutex_lock(&server->clientMutexs[cmd->fd % server->workerCount]);
			server->clients[cmd->fd % server->workerCount].push_back(new Client(cmd->fd, "", cmd->port));
			pthread_mutex_unlock(&server->clientMutexs[cmd->fd % server->workerCount]);
			break;
		case DISCONNECT:
			server->OnDisconnect(cmd->fd, cmd->port);
			close(cmd->fd);
			break;
		case PROXY:
			server->OnProxyRecv(cmd->fd, cmd->str, cmd->temp);
		default:
			break;
		}
		delete cmd;
	}

	return arg;
}

void AServer::pushCommand(Command *cmd)
{
	pthread_mutex_lock(&update_mutex);
	commands.push(cmd);
	pthread_mutex_unlock(&update_mutex);
}

void AServer::run(std::string ip, std::vector<int> ports, size_t workerCount)
{
	workThreadNo = 0;

	this->workerCount = workerCount;
	if (ports.empty())
		throw Exception("AServer: no port");
	in_addr_t inet_ip = inet_addr(ip.c_str());
	if (inet_ip == INADDR_NONE)
		throw Exception("AServer: wrong ip");
	this->ports = ports;
	
	clients = new std::vector<Client*>[workerCount];
	clientMutexs = new pthread_mutex_t[workerCount];
	for (size_t i = 0;i < workerCount;i++)
		pthread_mutex_init(clientMutexs + i, NULL);

	pthread_mutex_init(&workno_mutex, NULL);
	pthread_mutex_init(&update_mutex, NULL);

	listenSocks.clear();
	for (size_t i = 0;i < ports.size();i++)
	{
		int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket < 0)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			throw Exception("AServer: socket error");
		}
		fcntl(listenSocket, F_SETFL, O_NONBLOCK);

		sockaddr_in servAddr;
		utils::memset(&servAddr, 0, sizeof(sockaddr_in));
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = utils::htons(ports[i]);
		servAddr.sin_addr.s_addr = inet_addr(ip.c_str());
		int option = 1;
		setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		if (bind(listenSocket, (sockaddr*)&servAddr, sizeof(sockaddr_in)) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw Exception("AServer: bind error");
		}
		if (listen(listenSocket, 3000) == -1)
		{
			for (i = 0;i < listenSocks.size();i++)
				close(listenSocks[i]);
			close(listenSocket);
			throw Exception("AServer: listen error");
		}

		listenSocks.push_back(listenSocket);
	}

	pthread_t acceptT;
	pthread_t updateT;
	pthread_t *workTs = new pthread_t[workerCount];
	pthread_create(&acceptT, NULL, AcceptThread, this);
	for (size_t i = 0;i < workerCount;i++)
		pthread_create(&workTs[i], NULL, WorkerThread, this);
	pthread_create(&updateT, NULL, UpdateThread, this);
	
	pthread_join(acceptT, NULL);
	delete[] workTs;

	for (size_t i = 0;i < workerCount;i++)
		pthread_mutex_destroy(clientMutexs + i);
	pthread_mutex_destroy(&workno_mutex);
	pthread_mutex_destroy(&update_mutex);
	delete[] clientMutexs;
	delete[] clients;

	for (size_t i = 0;i < listenSocks.size();i++)
		close(listenSocks[i]);
}

void AServer::disconnect(int fd)
{
	pthread_mutex_lock(&clientMutexs[fd % workerCount]);
	for (size_t i = 0;i < clients[fd % workerCount].size();i++)
	{
		if (clients[fd % workerCount][i]->fd == fd)
		{
			clients[fd % workerCount][i]->willDie = true;
			break;
		}
	}
	pthread_mutex_unlock(&clientMutexs[fd % workerCount]);
}

void AServer::sendStr(int fd, std::string const &str)
{
	pthread_mutex_lock(&clientMutexs[fd % workerCount]);
	for (size_t i = 0;i < clients[fd % workerCount].size();i++)
	{
		if (clients[fd % workerCount][i]->fd == fd)
		{
			clients[fd % workerCount][i]->str.append(str);
			break;
		}
	}
	pthread_mutex_unlock(&clientMutexs[fd % workerCount]);
}

void AServer::writeFile(int fd, std::string const &str, void *temp)
{
	for (size_t i = 0;i < writeFiles.size();i++)
	{
		if (writeFiles[i]->fd == fd)
		{
			return;
		}
	}
	if (str.size())
		writeFiles.push_back(new Command(WRITE, fd, 0, str, temp));
	else
		OnFileWrite(fd, temp);
}

void AServer::readFile(int fd, void *temp)
{
	for (size_t i = 0;i < readFiles.size();i++)
	{
		if (readFiles[i]->fd == fd)
		{
			return;
		}
	}
	readFiles.push_back(new Command(READ, fd, 0, "", temp));
}

struct ProxyData
{
	AServer *server;
	std::string url;
	int port;
	std::string str;
	void *temp;
};

void AServer::proxySend(std::string const &url, int port, std::string const &str, void *temp)
{
	pthread_t proxyT;
	ProxyData* pd = new ProxyData();
	pd->server = this;
	pd->url = url;
	pd->port = port;
	pd->str = str;
	pd->temp = temp;
	pthread_create(&proxyT, NULL, ProxyThread, pd);
	pthread_detach(proxyT);
}

void *AServer::ProxyThread(void *arg)
{
	ProxyData *pd = static_cast<ProxyData*>(arg);
	
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return NULL;

	sockaddr_in proxyAddress;
	proxyAddress.sin_family = AF_INET;
	proxyAddress.sin_addr.s_addr = ::inet_addr(pd->url.c_str());
	proxyAddress.sin_port = utils::htons(pd->port);

	if (connect(fd, (sockaddr*)&proxyAddress, sizeof(proxyAddress)) < 0)
		return NULL;

	write(fd, pd->str.c_str(), pd->str.size());
	Request req;
	while (true)
	{
		char buf[BUFSIZ];
		int str_len = read(fd, buf, BUFSIZ);
		if (str_len <= 0)
			break;
		std::string recvStr;
		req.add(std::string(buf, str_len));
		if (!req.needRecv())
			pd->server->pushCommand(new Command(PROXY, fd, 0, req.deserialize(), pd->temp));
	}
	std::cout << "sended?" << std::endl;
	close(fd);
	delete pd;
	return NULL;
}
