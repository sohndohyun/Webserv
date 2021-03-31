#include "EchoServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include <iostream>

void EchoServer::OnRecv(int fd, std::string const &str)
{
	using namespace std;
	(void)fd;

	cout << "-------str----------\n";
	cout << str << endl;
	cout << "===================\n";
	RequestParser req(str);
	Response res("jachoi server");
	switch (req.getMethodType())
	{
		case GET: // fallthrought
		{
			cout << "=path=====================================\n";
			cout <<  req.pathparser->path << endl;
			cout << "==========================================\n";
			if (req.pathparser->path == "/directory/oulalala")
			{
				sendStr(fd, res.makeResFromText(404, "ss"));
				disconnect(fd);
				return;
			}
			sendStr(fd, res.makeResFromText(200, "hello world"));
			disconnect(fd);
			break;
		}
		case POST:
		{
			sendStr(fd, res.makeResFromText(405, "1"));
			disconnect(fd);
			break;
		}
		case HEAD:
		{
			sendStr(fd, res.makeResFromText(405, "1"));
			disconnect(fd);
			break;
		}
	}
	// for (std::map<string, string>::iterator it = req.header.begin(); it != req.header.end() ; it++)
	// 	cout << it->first << ": " << it->second << endl;
	// cout << "body: " <<  req.body << endl;
}

void EchoServer::OnSend(int fd)
{
	(void)&fd;
}

void EchoServer::OnAccept(int fd, int port)
{
	(void)&fd;
	(void)&port;
	std::cout << fd << "(" << port << "): accepted!" << "\n";
}

void EchoServer::OnDisconnect(int fd)
{
	(void)&fd;
	std::cout << fd << ": disconnected!" << "\n";
}
