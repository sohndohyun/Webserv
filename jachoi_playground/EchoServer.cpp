#include "EchoServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include <iostream>

void EchoServer::OnRecv(int fd, std::string const &str)
{
	using namespace std;
	(void)fd;
	RequestParser req(str);
	
	switch (req.getMethodType())
	{
	case GET: // fallthrought
	case POST:
	case HEAD:
	case PUT:
	{
		Response res("jachoi server");
		cout << "method: " << req.method << endl;
		std::string content = res.makeResFromText(200, "hell world");
		sendStr(fd, content);
		cout << "===============content================" << endl;
		cout << content << endl;
		cout << "======================================" << endl;
		break;
	}
	default:
		cerr << "unsupported method type!" << endl;
		break;
	}
	cout << "================= orig =====================" << endl;
	cout << str << endl;
	cout << "================= parse =====================" << endl;
	
	for (std::map<string, string>::iterator it = req.header.begin(); it != req.header.end() ; it++)
		cout << it->first << ": " << it->second << endl;
	cout << "body: " <<  req.body << endl;
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
