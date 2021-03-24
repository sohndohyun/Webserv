#ifndef astatelessserver_hpp
#define astatelessserver_hpp

#include <string>
#include <exception>
#include <vector>

class AStatelessServer
{
public:
	class ServerException : public std::exception
	{
	private:
		std::string msg;
	public:
		ServerException(std::string const &msg) _NOEXCEPT;
		virtual ~ServerException() _NOEXCEPT;
		virtual char const *what() const _NOEXCEPT;
	};

protected:
	class Client
	{
	private:
		Client();
	public:
		Client(int, std::string const &);
		int fd;
		std::string str;
	};

private:
	std::vector<Client*> clients;  

public:
	AStatelessServer();
	virtual ~AStatelessServer();

	void run(std::string ip, std::vector<int> ports);
	void sendStr(int fd, std::string const &str);

	virtual void OnRecv(int fd, std::string const &str) = 0;
	virtual void OnSend(int fd) = 0;
	virtual void OnAccept(int fd, int port) = 0;
};

#endif