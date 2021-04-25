#ifndef ASEREVER_HPP
#define ASEREVER_HPP

#include <string>
#include <exception>
#include <vector>
#include <map>
#include <pthread.h>
#include <queue>
#ifdef BUFSIZ
#undef BUFSIZ
#define BUFSIZ 65535
#else
#define BUFSIZ 65535
#endif

class AServer
{
private:
	class Client
	{
	private:
		Client();
	public:
		Client(int, std::string const &, int);
		int fd;
		int port;
		bool willDie;
		std::string str;
	};

	enum CMDType
	{
		RECV, SEND, READ, WRITE, ACCEPT, DISCONNECT
	};

	class Command
	{
	private:
		Command();
	public:
		Command(CMDType, int, int, std::string const&, void *);
		CMDType type;
		int fd;
		int port;
		std::string str;
		void *temp;
	};

public:
	typedef struct s_analysis
	{
		std::map<std::string, int> referer;
		std::map<std::string, int> user_agent;
	}t_analysis;

private:

	std::vector<Client*> *clients;
	std::vector<Command*> writeFiles;
	std::vector<Command*> readFiles;
	std::vector<int> listenSocks;
	std::vector<int> ports;

	std::queue<Command*> commands;

	size_t workerCount;

	pthread_mutex_t *clientMutexs;
	static void *AcceptThread(void *arg);
	static void *WorkerThread(void *arg);
	static void *FileThread(void *arg);
	static void *UpdateThread(void *arg);

	void pushCommand(Command *cmd);

public:
	void run(std::string ip, std::vector<int> ports, size_t workerNo = 4);
	void sendStr(int fd, std::string const &str);
	void disconnect(int fd);

	void writeFile(int fd, std::string const &str, void *temp);
	void readFile(int fd, void *temp);

	virtual void OnRecv(int fd, int port, std::string const &str) = 0;
	virtual void OnSend(int fd, int port) = 0;
	virtual void OnAccept(int fd, int port) = 0;
	virtual void OnDisconnect(int fd, int port) = 0;

	virtual void OnFileRead(int fd, std::string const &str, void *temp) = 0;
	virtual void OnFileWrite(int fd, void *temp) = 0;
};

#endif