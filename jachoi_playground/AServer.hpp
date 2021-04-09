#ifndef AServer_hpp
#define AServer_hpp

#include <string>
#include <exception>
#include <vector>
#include "Client.hpp"

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

private:
	std::vector<Client*> clients;

public:
	AServer();
	virtual ~AServer();

	void run(std::string ip, std::vector<int> ports);
	void sendStr(Client& cl, std::string const &str);
	void disconnect(Client& cl);

	virtual void OnRecv(Client& cl) = 0;
	virtual void OnSend(Client& cl) = 0;
	virtual void OnAccept(Client& cl) = 0;
	virtual void OnDisconnect(Client& cl) = 0;
};

#endif
