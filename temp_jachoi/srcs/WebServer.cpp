#include "WebServer.hpp"
#include "Response.hpp"
#include <iostream>

void WebServer::OnRecv(int fd, std::string const &str)
{
	requests[fd]->add(str);
	if (requests[fd]->needRecv())
		return;
	request_process(fd, *requests[fd]);
	requests[fd]->init();
}

void WebServer::request_process(int fd, Request &req)
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
				res.setContentType(req.path);
				body.clear();
				cgi_stub("./cgi_tester", req, body);
				res.makeRes(body);
				sendStr(fd, res.res_str); 
				std::cout << "result =====\n" <<  res.res_str.substr(0, 200) << "\n============\n";
				break;
			}
			else if (req.path == "/post_body")
			{
				if (req.body.size() > 100)
				{
					res.setStatus(413);
					res.makeRes("bad request");
					sendStr(fd, res.res_str);
				}
				else
				{
					std::vector<char> v(req.body.size(), '1');
					res.setStatus(200);
					res.makeRes(std::string(v.begin(), v.end()));
					sendStr(fd, res.res_str);
				}
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

void WebServer::cgi_stub(std::string const &path, Request &req, std::string &result)
{
	int fdin = open(".tempIN", O_CREAT | O_TRUNC | O_RDWR, 0644);
	int fdout = open(".tempOUT", O_CREAT | O_TRUNC | O_RDWR, 0644);

	write(fdin, req.body.c_str(), req.body.size());
	std::cout << "req body size : " << req.body.size() << "\n";
	lseek(fdin, 0, SEEK_SET);
	
	pid_t pid = fork();
	if (pid < 0)
		throw Exception("cgi: fork error");
	else if (pid == 0)
	{
		std::map<std::string, std::string> map_env;
		map_env["REQUEST_METHOD"] = req.method;
		map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
		map_env["PATH_INFO"] = req.path;
		if (req.header.find("X-Secret-Header-For-Test") != req.header.end())
			map_env["HTTP_X_SECRET_HEADER_FOR_TEST"] = req.header["X-Secret-Header-For-Test"];
		char **envp = jachoi::mtostrarr(map_env);

		dup2(fdin, 0);
		dup2(fdout, 1);

		char *const *nll = NULL;
		execve(path.c_str(), nll, envp);
		std::cerr << "execve error : " << errno << std::endl;
		exit(1);
	}
	
	char buf[1000000];

	waitpid(pid, NULL, 0);
	lseek(fdout, 0, SEEK_SET);

	while (true)
	{
		int ret = read(fdout, buf, 999999);
		if (ret == -1)
			throw Exception("cgi read error");
		else if (ret == 0)
			break;
		buf[ret] = 0;
		result.append(buf);
	}
	close(fdin);
	close(fdout);
	result = result.substr(result.find("\r\n\r\n") + 4);
}

void WebServer::OnSend(int fd)
{
	// disconnect(fd);
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
