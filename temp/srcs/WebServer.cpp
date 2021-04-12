#include "WebServer.hpp"
#include "Response.hpp"
#include <iostream>

void WebServer::OnRecv(int fd, std::string const &str)
{
	requests[fd]->add(str);
	if (requests[fd]->needRecv())
		return;
	std::cout << "sending...\n";
	request_process(fd, *requests[fd]);
	requests[fd]->init();
	disconnect(fd);
}

void WebServer::request_process(int fd, Request const &req)
{
	Response res("webserv");
	std::string body = "<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>";
	std::string req_str = "GET / HTTP/1.1\r\n";
	res.setContentType("text.html");
	switch (req.methodType())
	{
		case GET: // fallthrought
		{
			if (req.path == "/directory/oulalala")
			{
				res.setStatus(404);
				res.makeRes("hello");
				sendStr(fd, res.res_str);
				return;
			}
			if (req.path == "/directory/nop/other.pouac")
			{
				res.setStatus(404);
				res.makeRes("hello");
				sendStr(fd, res.res_str);
				return;
			}
			if (req.path == "/directory/Yeah")
			{
				res.setStatus(404);
				res.makeRes("hello");
				sendStr(fd, res.res_str);
				return;
			}
			res.setStatus(200);
			res.makeRes(body);
			sendStr(fd, res.res_str);
			break;
		}
		case POST:
		{
			if (req.path == "/directory/youpi.bla" || req.path == "/directory/youpla.bla")
			{
				res.setStatus(req.errorCode);
				res.makeRes(cgi_stub(req.path, req.body));
				sendStr(fd, res.res_str);
				break;
			}
			res.setStatus(405);
			res.makeRes(body);
			sendStr(fd, res.res_str);
			break;
		}
		case HEAD:
		{
			res.setStatus(405);
			res.makeRes(body);
			sendStr(fd, res.res_str);
			break;
		}
		case PUT:
		{
			res.setStatus(200);
			res.makeRes(req.body);
			sendStr(fd, res.res_str);
			break;
		}
		default:
		{
			std::cout << "not code yet\n";
			break;
		}
	}
}

std::string const &cgi_stub(std::string const &path, std::string const &body)
{

}

void WebServer::OnSend(int fd)
{
	(void)&fd;
}

void WebServer::OnAccept(int fd, int port)
{
	std::cout << fd << "(" << port << "): accepted!" << "\n";
	requests.insert(std::pair<int, Request*>(fd, new Request));
}

void WebServer::OnDisconnect(int fd)
{
	std::cout << fd << ": disconnected!" << "\n";
	requests.erase(requests.find(fd));
}

WebServer::~WebServer()
{
	std::map<int, Request*>::iterator it = requests.begin();
	for(;it != requests.end();it++)
		delete it->second;
	requests.clear();
}
