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

size_t workThreadNo = 0;
pthread_mutex_t workno_mutex;
pthread_mutex_t file_mutex;
pthread_mutex_t update_mutex;

AServer::Client::Client(int fd, std::string const &str, int port) : fd(fd), port(port), willDie(false), str(str) {}
AServer::Command::Command(CMDType type, int fd, int port, std::string const &str, void *temp) : type(type), fd(fd), port(port), str(str), temp(temp){}

void *AServer::AcceptThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);
	int fdMax = 3;

	fd_set set;
	FD_ZERO(&set);
	for (size_t i = 0;i < server->listenSocks.size();i++)
	{
		if (fdMax < server->listenSocks[i])
			fdMax = server->listenSocks[i];
		FD_SET(server->listenSocks[i], &set);
	}
	while (true)
	{
		int selRet = select(fdMax + 1, &set, NULL, NULL, NULL);
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
					continue ;	// TODO: log when accept failed
				fcntl(clntSocket, F_SETFL, O_NONBLOCK);
				if (fdMax < clntSocket)
					fdMax = clntSocket;

				//size_t idx = clntSocket % server->workerCount;
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
//				close(cl->fd);
				delete cl;
				continue;
			}
			it++;
		}
		pthread_mutex_unlock(&server->clientMutexs[threadNo]);
	}
	return arg;
}

void *AServer::FileThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);
	int fdMax = 3;

	fd_set wset, rset;
	timeval tval;
	tval.tv_usec = 100;
	tval.tv_sec = 0;

	while (true)
	{
		FD_ZERO(&wset);
		FD_ZERO(&rset);
		pthread_mutex_lock(&file_mutex);
		for (size_t i = 0;i < server->writeFiles.size();i++)
		{
			if (fdMax < server->writeFiles[i]->fd)
				fdMax = server->writeFiles[i]->fd;
			FD_SET(server->writeFiles[i]->fd, &wset);
		}
		for (size_t i = 0;i < server->readFiles.size();i++)
		{
			if (fdMax < server->readFiles[i]->fd)
				fdMax = server->readFiles[i]->fd;
			FD_SET(server->readFiles[i]->fd, &rset);
		}
		pthread_mutex_unlock(&file_mutex);
		int selRet = select(fdMax + 1, &rset, &wset, NULL, &tval);
		if (selRet == -1)
		{
			std::cerr << "FileThread: " << strerror(errno) << "\n";
			break;
		}
		else if (selRet == 0)
			continue ;
		pthread_mutex_lock(&file_mutex);
		for (std::vector<Command*>::iterator it = server->readFiles.begin(); it != server->readFiles.end();)
		{
			Command *wf = *it;
			if (FD_ISSET(wf->fd, &rset))
			{
				char buf[BUFSIZ];
				int str_len = read(wf->fd, buf, BUFSIZ);
				if (str_len < 0)
				{
					it = server->readFiles.erase(it);
					delete wf;
					continue;
				}
				if (str_len == 0)
				{
					it = server->readFiles.erase(it);
					server->pushCommand(new Command(READ, wf->fd, 0, wf->str, wf->temp));
					delete wf;
					continue;
				}
				wf->str.append(buf, str_len);
			}
			it++;
		}
		for (std::vector<Command*>::iterator it = server->writeFiles.begin(); it != server->writeFiles.end();)
		{
			Command *wf = *it;
			if (FD_ISSET(wf->fd, &wset))
			{
				int ret = write(wf->fd, wf->str.c_str(), wf->str.size());
				if (ret <= 0)
				{
					it = server->writeFiles.erase(it);
					delete wf;
				}
				else
				{
					it = server->writeFiles.erase(it);
					server->pushCommand(new Command(WRITE, wf->fd, 0, "", wf->temp));
					delete wf;
				}
			}
		}
		
		pthread_mutex_unlock(&file_mutex);
	}

	return arg;
}

void *AServer::UpdateThread(void *arg)
{
	AServer* server = static_cast<AServer*>(arg);

	while (true)
	{
		pthread_mutex_lock(&update_mutex);
		if (server->commands.empty())
		{
			pthread_mutex_unlock(&update_mutex);
			continue;
		}
		Command* cmd = server->commands.front();
		server->commands.pop();
		pthread_mutex_unlock(&update_mutex);

		switch(cmd->type)
		{
		case RECV:
			server->OnRecv(cmd->fd, cmd->port, cmd->str);
			break;
		case SEND:
			server->OnSend(cmd->fd, cmd->port);
			break;
		case READ:
			server->OnFileRead(cmd->fd, cmd->str, cmd->temp);
			break;
		case WRITE:
			server->OnFileWrite(cmd->fd, cmd->temp);
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
	pthread_mutex_init(&file_mutex, NULL);
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
	pthread_t fileT;
	pthread_t updateT;
	pthread_t *workTs = new pthread_t[workerCount];
	pthread_create(&acceptT, NULL, AcceptThread, this);
	pthread_create(&fileT, NULL, FileThread, this);
	for (size_t i = 0;i < workerCount;i++)
		pthread_create(&workTs[i], NULL, WorkerThread, this);
	pthread_create(&updateT, NULL, UpdateThread, this);
	
	pthread_join(acceptT, NULL);
	delete[] workTs;

	for (size_t i = 0;i < workerCount;i++)
		pthread_mutex_destroy(clientMutexs + i);
	pthread_mutex_destroy(&workno_mutex);
	pthread_mutex_destroy(&file_mutex);
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
	pthread_mutex_lock(&file_mutex);
	for (size_t i = 0;i < writeFiles.size();i++)
	{
		if (writeFiles[i]->fd == fd)
		{
			pthread_mutex_unlock(&file_mutex);
			return;
		}
	}
	if (str.size())
	{
		writeFiles.push_back(new Command(WRITE, fd, 0, str, temp));
		pthread_mutex_unlock(&file_mutex);
	}
	else
	{
		pthread_mutex_unlock(&file_mutex);
		pushCommand(new Command(WRITE, fd, 0, "", temp));
	}
}

void AServer::readFile(int fd, void *temp)
{
	pthread_mutex_lock(&file_mutex);
	for (size_t i = 0;i < readFiles.size();i++)
	{
		if (readFiles[i]->fd == fd)
		{
			pthread_mutex_unlock(&file_mutex);
			return;
		}
	}
	readFiles.push_back(new Command(READ, fd, 0, "", temp));
	pthread_mutex_unlock(&file_mutex);
}
