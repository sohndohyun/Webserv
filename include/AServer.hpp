#ifndef AServer_hpp
#define AServer_hpp

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
		bool willDie;
		bool sent;
		int lasttime;
		std::string str;
		bool isTimeout();
	};

public:
	typedef struct s_analysis
	{
		std::map<std::string, int> referer;
		std::map<std::string, int> user_agent;
	}t_analysis;

private:
	std::vector<Client*> clients;

public:
	void run(std::string ip, std::vector<int> ports);
	void sendStr(int fd, std::string const &str);
	void disconnect(int fd);

	virtual void OnRecv(int fd, std::string const &str) = 0;
	virtual void OnSend(int fd) = 0;
	virtual void OnAccept(int fd, int port) = 0;
	virtual void OnDisconnect(int fd) = 0;
};

#endif