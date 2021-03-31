#include "WebServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include <iostream>

void WebServer::OnRecv(int fd, std::string const &str)
{
	reqStr.append(str);
	std::cout << "-------str----------\n";
	std::cout << reqStr << std::endl;
	std::cout << "===================\n";
	RequestParser req(reqStr);
	if (req.needRecvMore())
	{
		return ;
	}
	request_process(fd, req);
	reqStr.clear();
	disconnect(fd);
}

void WebServer::request_process(int fd, RequestParser const &req)
{
	Response res("web server");
	switch (req.getMethodType())
	{
		case GET: // fallthrought
		{
			std::cout << "=path=====================================\n";
			std::cout <<  req.pathparser->path << std::endl;
			std::cout << "=path=====================================\n";
			if (req.pathparser->path == "/directory/oulalala")
			{
				sendStr(fd, res.makeResFromText(404, "ss"));
				disconnect(fd);
				return;
			}
			sendStr(fd, res.makeResFromText(200, "hello world"));
			break;
		}
		case POST:
		{
			sendStr(fd, res.makeResFromText(405, "1"));
			break;
		}
		case HEAD:
		{
			sendStr(fd, res.makeResFromText(405, "1"));
			break;
		}
	}
}

void WebServer::OnSend(int fd)
{
	(void)&fd;
}

void WebServer::OnAccept(int fd, int port)
{
	(void)&fd;
	(void)&port;
	std::cout << fd << "(" << port << "): accepted!" << "\n";
}

void WebServer::OnDisconnect(int fd)
{
	(void)&fd;
	std::cout << fd << ": disconnected!" << "\n";
}
