#include "WebServer.hpp"
#include "Response.hpp"
#include <iostream>
#include "FileIO.hpp"
#include "ConfigCheck.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
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

WebServer::WebServer(ConfigParse &conf): confs(conf){}

void WebServer::OnRecv(int fd, int port, std::string const &str)
{
	requests[fd]->add(str);
	if (requests[fd]->needRecv())
		return;
	request_process(fd, port, *requests[fd]);
	if ((requests[fd]->header["Connection"] == "close"))
		disconnect(fd);
	requests[fd]->init();
}

void WebServer::request_process(int fd, int port, Request &req)
{
	Response *res = new Response(confs.conf[get_conf_idx(port)].server.name);
	req.isReferer(analysis);
	req.isUserAgent(analysis);
	if (req.errorCode != 200)
	{
		errorRes(fd, port, res, req.errorCode);
		sendStr(fd, res->res_str);
		return ;
	}

	switch (req.methodType())
	{
		case GET:
		{
			methodGET(fd, port, res, req);
			break;
		}
		case POST:
		{
			methodPOST(fd, port, res, req);
			break;
		}
		case HEAD:
		{
			methodHEAD(fd, port,res, req);
			break;
		}
		case PUT:
		{
			methodPUT(fd, port,res, req);
			break;
		}
		default:
			errorRes(fd, port, res, 503);
			break;
	}
}

int WebServer::cgi_stub(int tempfd, FileData *fData)
{
	lseek(tempfd, 0, SEEK_SET);

	int fdout = utils::open(".TEMPOUT", O_CREAT | O_TRUNC | O_RDWR, 0644);
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

void WebServer::OnSend(int fd, int port)
{
	(void)fd;
	(void)port;
}

void WebServer::OnAccept(int fd, int port)
{
	(void)&port;
	requests.insert(std::make_pair(fd, new Request()));
}

void WebServer::OnDisconnect(int fd, int port)
{
	(void)port;
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
		writeFile(utils::open(fData->path.c_str(), O_CREAT | O_WRONLY, 0644), s, fData);
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

void WebServer::methodGET(int fd, int port,  Response *res, Request &req)
{
	ConfigCheck cfg_check(confs.conf[get_conf_idx(port)], req.path);
	std::vector<std::string> allow_methods;
	std::string body = "";
	struct stat sb;

	if (cfg_check.analysisCheck())
	{
		res->setContentType(".html");
		res->setStatus(200);
		res->makeRes(cfg_check.makeAnalysisHTML(analysis));
		sendStr(fd, res->res_str);
		delete res; return;
	}

	int is_dir = 0;
	std::string path = cfg_check.makeFilePath(is_dir);
	req.isAcceptLanguage(path, is_dir);

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(fd, port, res, 401);
	else if (path == "")
		errorRes(fd, port, res, 404);
	else if (cfg_check.methodCheck("GET", allow_methods) == false)
		errorRes(fd, port, res, 405, allow_methods);
	else
	{
		res->setContentType(path);
		res->setStatus(200);
		res->setContentLocation(req.path);

		if (stat(cfg_check.findPath().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			body = cfg_check.autoIdxCheck(port);
			if (body == "")
			{
				res->setLastModified(path);
				readFile(utils::open(path.c_str(), O_RDONLY), new FileData(fd, res));
				return ;
			}
			res->makeRes(body);
			sendStr(fd, res->res_str);
			delete res;
		}
		else
		{
			res->setLastModified(path);
			readFile(utils::open(path.c_str(), O_RDONLY), new FileData(fd, res));
		}
	}
}

void WebServer::methodHEAD(int fd, int port, Response *res, Request &req)
{
	ConfigCheck cfg_check(confs.conf[get_conf_idx(port)], req.path);
	std::vector<std::string> allow_methods;

	int is_dir = 0;
	std::string path = cfg_check.makeFilePath(is_dir);
	req.isAcceptLanguage(path, is_dir);

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(fd, port, res, 401);
	else if (path == "")
		errorRes(fd, port, res, 404);
	else if (cfg_check.methodCheck("HEAD", allow_methods) == false)
		errorRes(fd, port, res, 405, allow_methods);
	else
	{
		res->setContentType(path);
		res->setContentLocation(req.path);
		res->setStatus(200);
		res->makeRes("");
		sendStr(fd, res->res_str);
		delete res;
	}
}

void WebServer::methodPUT(int fd, int port, Response *res, Request &req)
{
	ConfigCheck cfg_check(confs.conf[get_conf_idx(port)], req.path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);
	std::vector<std::string> allow_methods;

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(fd, port, res, 401);
	else if (cfg_check.methodCheck("PUT", allow_methods) == false)
		errorRes(fd, port, res, 405, allow_methods);
	else
	{
		if (stat_rtn == -1)
		{
			res->setStatus(201);
			res->setLocation(req.path);
			res->setLastModified(path);
			res->makeRes("", true);
			writeFile(utils::open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644), req.body, new FileData(fd, res));
		}
		else if (stat_rtn == 0 && S_ISREG(sb.st_mode))
		{
			res->setStatus(200);
			res->setContentLocation(req.path);
			res->setLastModified(path);
			res->makeRes("", true);
			writeFile(utils::open(path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644), req.body, new FileData(fd, res));
		}
	}
}

void WebServer::methodPOST(int fd, int port, Response *res, Request &req)
{
	ConfigParse::t_conf conf = confs.conf[get_conf_idx(port)];
	ConfigCheck cfg_check(conf, req.path);
	std::string path = cfg_check.findPath();
	//struct stat sb;
	//int stat_rtn = stat(path.c_str(), &sb);
	std::vector<std::string> allow_methods;

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(fd, port, res, 401);
	else if (cfg_check.methodCheck("POST", allow_methods) == false)
		errorRes(fd, port, res, 405, allow_methods);
	else if (cfg_check.client_max_body_size_Check(req.body.size()) == false)
		errorRes(fd, port, res, 413);
	else
	{
		if (cfg_check.cgiCheck())
		{
			std::string tempfile = ".TEMP";
			res->setContentType(path);
			res->setStatus(200);
			res->setContentLocation(req.path);
			res->setLastModified(path);
		
			std::map<std::string, std::string> map_env = utils::set_cgi_enviroment(conf, req, path, port);
			writeFile(utils::open(tempfile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644), req.body, 
				new FileData(fd, res, true, utils::mtostrarr(map_env), path));
			}
		else
		{
			res->setContentType(path);
			res->setStatus(200);
			res->setContentLocation(req.path);
			res->setLastModified(path);
			res->makeRes(req.body);
			writeFile(utils::open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644), req.body, new FileData(fd, res));
		}
	}
}

void WebServer::errorRes(int fd, int port, Response *res, int errorCode, const std::vector<std::string>& allow_methods)
{
	ConfigParse::t_conf& conf = confs.conf[get_conf_idx(port)];
	std::string path = conf.server.error_root + conf.server.error_page[errorCode];

	res->setStatus(errorCode);
	res->setContentType(path);
	switch (errorCode)
	{
		case 401:
		{
			res->setWWWAuthenticate();
			break;
		}
		case 405:
		{
			res->setAllow(allow_methods);
			break;
		}
		case 503:
		{
			res->setRetryAfter();
			break;
		}
	}
	readFile(utils::open(path.c_str(), O_RDONLY), new FileData(fd, res));
}

int WebServer::get_conf_idx(int port)
{
	for (size_t i = 0 ; i < confs.conf.size(); i++)
	{
		std::vector<int>::iterator it = std::find(confs.conf[i].server.port.begin(), confs.conf[i].server.port.end(), port);
		if (it != confs.conf[i].server.port.end())
			return i;
	}
	return -1;
}