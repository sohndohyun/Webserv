#ifndef AServer_hpp
#define AServer_hpp

#include <string>
#include <exception>
#include <vector>

class AServer
{
public:
	class ServerException : public std::exception
	{
	private:
		std::string msg;
	public:
		ServerException(std::string const &msg) throw();
		virtual ~ServerException() throw();
		virtual char const *what() const throw();
	};

protected:
	class Client
	{
	private:
		Client();
	public:
		Client(int, std::string const &);
		int fd;
		bool willDie;
		std::string str;
	};

private:
	std::vector<Client*> clients;  

public:
	AServer();
	virtual ~AServer();

	void run(std::string ip, std::vector<int> ports);
	void sendStr(int fd, std::string const &str);
	void disconnect(int fd);

	virtual void OnRecv(int fd, std::string const &str) = 0;
	virtual void OnSend(int fd) = 0;
	virtual void OnAccept(int fd, int port) = 0;
	virtual void OnDisconnect(int fd) = 0;
};

#endif