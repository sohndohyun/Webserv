#ifndef ASEREVER_HPP
#define ASEREVER_HPP

#include <string>
#include <exception>
#include <vector>
#include <map>
#ifdef BUFSIZ
#undef BUFSIZ
#define BUFSIZ 65535
#else
#define BUFSIZ 65535
#endif

class AServer
{
protected:
	class Client
	{
	private:
		Client();
	public:
		Client(int, std::string const &);
		int fd;
		int port;
		bool willDie;
		std::string str;
	};

	class Workfile
	{
	private:
		Workfile();
	public:
		Workfile(int, std::string const &, void *);
		int fd;
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
	std::vector<Client*> clients;
	std::vector<Workfile*> writeFiles;
	std::vector<Workfile*> readFiles;

public:
	void run(std::string ip, std::vector<int> ports);
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