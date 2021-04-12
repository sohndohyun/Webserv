#include "WebServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ConfigParse.hpp"
#include "FileIO.hpp"
#include "ConfigCheck.hpp"
#include "ChunkParser.hpp"
#include "CGIStub.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

WebServer::WebServer(ConfigParse &conf): conf(conf){}

void WebServer::OnRecv(int fd, std::string const &str)
{
	reqStr[fd] += str;
	RequestParser req(reqStr[fd]);
	if (req.needRecvMore())
	{
		return ;
	}
	if (req.header["Transfer-Encoding"] == "chunked")
	{
		ChunkParser chunk(req.body);
		req.body = chunk.getData();
	}

	std::cout << "-------request----------\n";
	std::cout << reqStr[fd].substr(0, 500) << std::endl;
	std::cout << "===================\n";

	request_process(fd, req);
	std::cout << "fd : " << fd << std::endl;
	reqStr[fd].clear();
	disconnect(fd);
}

void WebServer::request_process(int fd, RequestParser const &req)
{
	Response res(conf.server->name);
	switch (req.getMethodType())
	{
		case GET: // fallthrought
		{
			methodGET(res, req.pathparser->path);
			break;
		}
		case POST:
		{
			methodPOST(res, req);
			break;
		}
		case HEAD:
		{
			methodHEAD(res, req.pathparser->path);
			break;
		}
		case PUT:
		{
			methodPUT(res, req);
			break;
		}
	}
	std::cout << "----------response---------" << std::endl;
	std::cout << res.res_str.substr(0, 500) << std::endl;
	std::cout << "---------------------------" << std::endl;
	sendStr(fd, res.res_str);
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
	std::cout << fd << ": disconnected!" << "\n\n";
}

void WebServer::methodGET(Response &res, std::string req_path)
{
	ConfigCheck cfg_check(conf, req_path);
	std::string body = "";
	std::string path = cfg_check.makeFilePath();
	struct stat sb;

	res.setContentType(path);
	if (cfg_check.methodCheck("GET") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
		res.setContentType(path);
		body = jachoi::FileIO(path).read();
	}
	else if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res.setStatus(404);
		res.setContentType(path);
		body = jachoi::FileIO(path).read();
	}
	else
	{
		res.setStatus(200);
		if (stat(cfg_check.findPath().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			body = cfg_check.autoIdxCheck();
		else
			body = jachoi::FileIO(path).read();
	}
	res.makeRes(body);
}

void WebServer::methodHEAD(Response &res, std::string req_path)
{
	ConfigCheck cfg_check(conf, req_path);
	std::string body;
	std::string path = cfg_check.makeFilePath();

	res.setContentType(path);
	if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res.setStatus(404);
		res.setContentType(path);
		body = jachoi::FileIO(path).read();
	}
	else if (cfg_check.methodCheck("HEAD") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
		res.setContentType(path);
		body = jachoi::FileIO(path).read();
	}
	else
	{
		res.setStatus(200);
		body = "";
	}
	res.makeRes(body);
}

void WebServer::methodPUT(Response &res, RequestParser const &req)
{
	ConfigCheck cfg_check(conf, req.pathparser->path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);

	res.setContentType(path);
	if (cfg_check.methodCheck("PUT") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
		res.setContentType(path);
		body = jachoi::FileIO(path).read();
	}
	else if (stat_rtn == -1)
	{
		jachoi::FileIO(path).write(req.body);
		res.setStatus(200);
		body = req.body;
	}
	else if (stat_rtn == 0 && S_ISREG(sb.st_mode))
	{
		jachoi::FileIO(path).append(req.body);
		res.setStatus(200);
		body = jachoi::FileIO(path).read();
	}
	res.makeRes(body);
}

void WebServer::methodPOST(Response &res, RequestParser const &req)
{
	ConfigCheck cfg_check(conf, req.pathparser->path);
	std::string body;
	std::string path = cfg_check.findPath();
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);

	res.setContentType(path);
	if (cfg_check.methodCheck("POST") == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
		res.setContentType(path);
	}
	else if (cfg_check.client_max_body_size_Check(req.body.size()) == false)
	{
		path = conf.server->error_root + conf.server->error_page[413];
		res.setStatus(413);
		res.setContentType(path);
	}
	else if (path.substr(path.rfind('.') + 1) == "bla")
	{
		body = CGIStub(req, path).getCGIResult();
		jachoi::FileIO(path).write(body);
		res.setStatus(200);
		res.setContentLocation(req.pathparser->path);
	}
	else
	{
		if (stat_rtn == 0 && S_ISDIR(sb.st_mode))
			path += "post_file";
		jachoi::FileIO(path).write(req.body);
		res.setStatus(200);
		res.setContentLocation(req.pathparser->path);
	}
	body = jachoi::FileIO(path).read();
	res.makeRes(body);
}