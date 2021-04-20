#include "WebServer.hpp"
#include "Response.hpp"
#include <iostream>
#include "ConfigParse.hpp"
#include "FileIO.hpp"
#include "ConfigCheck.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

WebServer::FileData::FileData(int fd, Response *res, bool isCGI, char **envp, std::string const &path) :
	fd(fd), res(res), isCGI(isCGI), envp(envp), path(path) {}
WebServer::FileData::~FileData() 
{ 
	if (res) 
		delete res;
	if (envp)
	{
		char **temp = envp;
		while (*temp)
		{
			delete[] *temp;
			temp++;
		}
		delete[] envp;
	}
}

WebServer::WebServer(ConfigParse &conf): conf(conf){}

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
	Response *res = new Response(conf.server->name);
	switch (req.methodType())
	{
		case GET:
		{
			methodGET(fd, res, req.path);
			break;
		}
		case POST:
		{
			methodPOST(fd, res, req);
			break;
		}
		case HEAD:
		{
			methodHEAD(fd, res, req.path);
			break;
		}
		case PUT:
		{
			methodPUT(fd, res, req);
			break;
		}
		default:
			methodInvalid(fd, res, req);
			break;
	}
}

int WebServer::cgi_stub(int tempfd, FileData *fData)
{
	lseek(tempfd, 0, SEEK_SET);

	int fdout = open(".TEMPOUT", O_CREAT | O_TRUNC | O_RDWR, 0644);
	pid_t pid = fork();
	if (pid < 0)
		throw Exception("cgi: fork error");
	else if (pid == 0)
	{
		dup2(tempfd, 0);
		dup2(fdout, 1);

		char *const *nll = NULL;
		execve(CGI_PATH, nll, fData->envp);
		exit(1);
	}

	waitpid(pid, NULL, 0);
	std::cout << fData->fd << ": cgi ended\n";
	lseek(fdout, 0, SEEK_SET);
	return fdout;
}

void WebServer::OnSend(int fd)
{
	std::cout << fd << ": sended!\n";
}

void WebServer::OnAccept(int fd, int port)
{
	std::cout << fd << "(" << port << "): accepted!" << "\n";
	requests.insert(std::make_pair(fd, new Request()));
}

void WebServer::OnDisconnect(int fd)
{
	std::cout << fd << ": disconnected!" << "\n";
	std::map<int, Request*>::iterator it = requests.find(fd);
	if (it != requests.end())
	{
		delete it->second;
		requests.erase(it);
	}
}

void WebServer::OnFileRead(int fd, std::string const &str, void *temp)
{
	FileData *fData = static_cast<FileData*>(temp);

	if (!fData->isCGI)
	{
		fData->res->makeRes(str);
		sendStr(fData->fd, fData->res->res_str);
		delete fData;
	}
	else
	{
		std::string s = str.substr(str.find("\r\n\r\n") + 4);
		fData->res->makeRes(s);
		fData->isCGI = false;
		writeFile(open(fData->path.c_str(), O_CREAT | O_WRONLY, 0644), s, fData);
	}
	close(fd);
}

void WebServer::OnFileWrite(int fd, void *temp)
{
	FileData *fData = static_cast<FileData*>(temp);

	if (fData->isCGI)
	{
		readFile(cgi_stub(fd, fData), fData);
	}
	else
	{
		sendStr(fData->fd, fData->res->res_str);
		delete fData;
	}
	close(fd);
}

WebServer::~WebServer()
{
	std::map<int, Request*>::iterator it = requests.begin();
	for(;it != requests.end();it++)
		delete it->second;
	requests.clear();
}

void WebServer::methodGET(int fd, Response *res, std::string req_path)
{
	ConfigCheck cfg_check(conf, req_path);
	std::string body = "";
	std::string path = cfg_check.makeFilePath();
	struct stat sb;

	res->setContentType(path);
	if (cfg_check.methodCheck("GET") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res->setStatus(405);
		res->setContentType(path);
		readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res ));
	}
	else if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res->setStatus(404);
		res->setContentType(path);
		readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res ));
	}
	else
	{
		res->setStatus(200);
		if (stat(cfg_check.findPath().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			body = cfg_check.autoIdxCheck();
			if (body == "")
				readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res));
			else
			{
				res->makeRes(body);
				sendStr(fd, res->res_str);
				delete res;
			}
		}
		else
			readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res ));
	}
}

void WebServer::methodHEAD(int fd, Response *res, std::string req_path)
{
	ConfigCheck cfg_check(conf, req_path);
	std::string body = "";
	std::string path = cfg_check.makeFilePath();

	res->setContentType(path);
	if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res->setStatus(404);
		res->setContentType(path);
		readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res));
	}
	else if (cfg_check.methodCheck("HEAD") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res->setStatus(405);
		res->setContentType(path);
		readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res));
	}
	else
	{
		res->setStatus(200);
		res->makeRes(body);
		sendStr(fd, res->res_str);
		delete res;
	}
}

void WebServer::methodPUT(int fd, Response *res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);

	res->setContentType(path);
	if (cfg_check.methodCheck("PUT") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res->setStatus(405);
		res->setContentType(path);
		readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res));
	}
	else if (stat_rtn == -1)
	{
		res->setStatus(200);
		res->makeRes(req.body);
		writeFile(open(path.c_str(), O_CREAT | O_WRONLY, 0644), req.body, new FileData(fd, res));
	}
	else if (stat_rtn == 0 && S_ISREG(sb.st_mode))
	{
		res->setStatus(200);
		jachoi::FileIO(path).append(req.body);
		jachoi::FileIO(path).read(body);
		res->makeRes(req.body);
		sendStr(fd, res->res_str);
		delete res;
	}
	else
	{
		res->makeRes(body);
		sendStr(fd, res->res_str);
		delete res;
	}
}

void WebServer::methodPOST(int fd, Response *res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);

	res->setContentType(path);
	if (cfg_check.methodCheck("POST") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res->setStatus(405);
		res->setContentType(path);
		res->makeRes(req.body);
		sendStr(fd, res->res_str);
		delete res;
	}
	else if (cfg_check.client_max_body_size_Check(req.body.size()) == false)
	{
		std::cerr << "error" << std::endl;
		path = conf.server->error_root + conf.server->error_page[413];
		res->setStatus(413);
		res->setContentType(path);
		res->makeRes(req.body);
		sendStr(fd, res->res_str);
		delete res;
	}
	else if (path.substr(path.rfind('.') + 1) == "bla")
	{
		std::string tempfile = ".TEMP";
		res->setStatus(200);
		
		std::map<std::string, std::string> map_env;
		map_env["REQUEST_METHOD"] = req.method;
		map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
		map_env["PATH_INFO"] = req.path;
		if (req.header.find("X-Secret-Header-For-Test") != req.header.end())
			map_env["HTTP_X_SECRET_HEADER_FOR_TEST"] = req.header["X-Secret-Header-For-Test"];
		
		writeFile(open(tempfile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644), req.body, 
			new FileData(fd, res, true, jachoi::mtostrarr(map_env), path));
	}
	else
	{
		if (stat_rtn == 0 && S_ISDIR(sb.st_mode))
			path += "post_file";

		res->setStatus(200);
		res->setContentLocation(req.path);
		res->makeRes(req.body);
		writeFile(open(path.c_str(), O_CREAT | O_WRONLY, 0644), req.body, new FileData(fd, res));
	}
}

void WebServer::methodInvalid(int fd, Response *res, Request &req)
{
	(void)req;
	std::string path = conf.server->error_root + conf.server->error_page[405];
	res->setStatus(405);
	res->setContentType(path);
	readFile(open(path.c_str(), O_RDONLY), new FileData(fd, res));
}
