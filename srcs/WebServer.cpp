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
#include <sys/wait.h>

WebServer::WebServer(ConfigParse::t_conf &conf): conf(conf){}

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
	Response res(conf.server.name);
	if (req.errorCode != 200)
	{
		errorRes(res, req.errorCode);
		sendStr(fd, res.res_str);
		return ;
	}

	switch (req.methodType())
	{
		case GET:
		{
			methodGET(res, req);
			break;
		}
		case POST:
		{
			methodPOST(res, req);
			break;
		}
		case HEAD:
		{
			methodHEAD(res, req);
			break;
		}
		case PUT:
		{
			methodPUT(res, req);
			break;
		}
		default:
			errorRes(res, 503);
			break;
	}
	//std::cout << "--------response str---------" << std::endl;
	//std::cout << res.res_str << std::endl;
	//std::cout << "--------response str---------" << std::endl;
	sendStr(fd, res.res_str);
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
	(void)fd;
	// disconnect(fd);
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

WebServer::~WebServer()
{
	std::map<int, Request*>::iterator it = requests.begin();
	for(;it != requests.end();it++)
		delete it->second;
	requests.clear();
}




void WebServer::methodGET(Response &res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::vector<std::string> allow_methods;
	std::string body = "";
	struct stat sb;

	int is_dir = 0;
	std::string path = cfg_check.makeFilePath(is_dir);
	req.isAcceptLanguage(path, is_dir);

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(res, 401);
	else if (path == "")
		errorRes(res, 404);
	else if (cfg_check.methodCheck("GET", allow_methods) == false)
		errorRes(res, 405, allow_methods);
	else
	{
		res.setContentType(path);
		res.setStatus(200);
		res.setContentLocation(req.path);
		if (stat(cfg_check.findPath().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			body = cfg_check.autoIdxCheck();
			if (body == "")
			{
				jachoi::FileIO(path).read(body);
				res.setLastModified(path);
			}
		}
		else
		{
			jachoi::FileIO(path).read(body);
			res.setLastModified(path);
		}
		res.makeRes(body);
	}
}

void WebServer::methodHEAD(Response &res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::vector<std::string> allow_methods;

	int is_dir = 0;
	std::string path = cfg_check.makeFilePath(is_dir);
	req.isAcceptLanguage(path, is_dir);

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(res, 401);
	else if (path == "")
		errorRes(res, 404);
	else if (cfg_check.methodCheck("HEAD", allow_methods) == false)
		errorRes(res, 405, allow_methods);
	else
	{
		res.setContentType(path);
		res.setContentLocation(req.path);
		res.setStatus(200);
		res.makeRes("");
	}
}

void WebServer::methodPUT(Response &res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);
	std::vector<std::string> allow_methods;

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(res, 401);
	else if (cfg_check.methodCheck("PUT", allow_methods) == false)
		errorRes(res, 405, allow_methods);
	else
	{
		if (stat_rtn == -1)
		{
			jachoi::FileIO(path).write(req.body);
			res.setStatus(201);
			res.setLocation(req.path);
		}
		else if (stat_rtn == 0 && S_ISREG(sb.st_mode))
		{
			jachoi::FileIO(path).append(req.body);
			res.setStatus(200);
			res.setContentLocation(req.path);
		}
		res.setLastModified(path);
		res.makeRes("", true);
	}
}

void WebServer::methodPOST(Response &res, Request &req)
{
	ConfigCheck cfg_check(conf, req.path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);
	std::vector<std::string> allow_methods;

	if (cfg_check.AuthorizationCheck(req.header["Authorization"]) == false)
		errorRes(res, 401);
	else if (cfg_check.methodCheck("POST", allow_methods) == false)
		errorRes(res, 405, allow_methods);
	else if (cfg_check.client_max_body_size_Check(req.body.size()) == false)
		errorRes(res, 413);
	else
	{
		if (cfg_check.cgiCheck())
			cgi_stub(CGI_PATH, req, body);
		else if (stat_rtn == 0 && S_ISDIR(sb.st_mode))
		{
			path += "post_file";
			if (req.path[req.path.length() - 1] != '/')
				req.path += '/';
			req.path += "post_file";
		}
		jachoi::FileIO(path).write(req.body);
		res.setContentType(path);
		res.setStatus(200);
		res.setContentLocation(req.path);
		res.setLastModified(path);
		res.makeRes(body);
	}
}

void WebServer::errorRes(Response &res, int errorCode, std::vector<std::string> allow_methods)
{
	std::string body;
	std::string path = conf.server.error_root + conf.server.error_page[errorCode];

	jachoi::FileIO(path).read(body);
	res.setStatus(errorCode);
	res.setContentType(path);
	switch (errorCode)
	{
		case 401:
		{
			res.setWWWAuthenticate();
			break;
		}
		case 405:
		{
			res.setAllow(allow_methods);
			break;
		}
		case 503:
		{
			res.setRetryAfter();
			break;
		}
	}
	res.makeRes(body);
}
