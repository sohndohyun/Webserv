#include "WebServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ConfigParse.hpp"
#include "FileIO.hpp"
#include "ConfigCheck.hpp"
#include "ChunkParser.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

WebServer::WebServer(ConfigParse &conf): conf(conf){}

void WebServer::OnRecv(int fd, std::string const &str)
{
	reqStr.append(str);
try{
		RequestParser req2(reqStr);
}
catch(std::exception &e) {std::cout << "reqStr : " << reqStr << std::endl;}

	RequestParser req(reqStr);
	if (req.needRecvMore())
	{
		return ;
	}
	if (req.header["Transfer-Encoding"] == "chunked")
	{
		ChunkParser chunk(req.body);
		req.body = chunk.getData();
	}

	std::cout << "-------str----------\n";
	std::cout << reqStr << std::endl;
	std::cout << "===================\n";

	request_process(fd, req);
	reqStr.clear();
	disconnect(fd);
}

void WebServer::request_process(int fd, RequestParser const &req)
{
	//ConfigParse conf;
	Response res(conf.server->name);
	std::cout << "=path=====================================\n";
	std::cout <<  req.pathparser->path << std::endl;
	std::cout << "=path=====================================\n";
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
	std::cout << res.res_str << std::endl;
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
	ConfigCheck cfg_check(conf);
	std::string body;
	std::string path = cfg_check.makeFilePath(req_path);

	res.setContentType(path);
	if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res.setStatus(404);
	}
	else if (cfg_check.methodCheck("GET", req_path) == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
	}
	else
		res.setStatus(200);
	body = jachoi::FileIO(path).read();
	res.makeRes(body);
}

void WebServer::methodHEAD(Response &res, std::string req_path)
{
	ConfigCheck cfg_check(conf);
	std::string body;
	std::string path = cfg_check.makeFilePath(req_path);

	res.setContentType(path);
	if (path == "")
	{
		path = conf.server->error_root + conf.server->error_page[404];
		res.setStatus(404);
		body = jachoi::FileIO(path).read();
	}
	else if (cfg_check.methodCheck("HEAD", req_path) == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
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
	ConfigCheck cfg_check(conf);
	std::string body;
	std::string path = cfg_check.findPath(req.pathparser->path);
	struct stat sb;

	res.setContentType(path);
	int stat_rtn = stat(path.c_str(), &sb);
	if (stat_rtn == 0 && S_ISDIR(sb.st_mode))
	{
		// 400 error?
	}
	else if (cfg_check.methodCheck("PUT", req.pathparser->path) == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
	}
	else if (stat_rtn == -1)
	{
		jachoi::FileIO(path).write(req.body);
		res.setStatus(201);
	}
	//else if (stat_rtn == 0 && S_ISREG(sb.st_mode) && req.body == "")
	//{
	//	res.setStatus(204);
	//}
	else if (stat_rtn == 0 && S_ISREG(sb.st_mode))
	{
		jachoi::FileIO(path).append(req.body);
		res.setStatus(200);
	}
	body = jachoi::FileIO(path).read();
	res.makeRes(body);
}


void WebServer::methodPOST(Response &res, RequestParser const &req)
{
	ConfigCheck cfg_check(conf);
	std::string body;
	std::string path = cfg_check.findPath(req.pathparser->path);
	struct stat sb;
	int stat_rtn = stat(path.c_str(), &sb);

	//post name???
	if (stat_rtn == 0 && S_ISDIR(sb.st_mode))
	{
		path += "/post1";
		stat_rtn = stat(path.c_str(), &sb);
	}

	res.setContentType(path);
	if (cfg_check.methodCheck("POST", req.pathparser->path) == false)
	{
		path = conf.server->error_root + conf.server->error_page[405];
		res.setStatus(405);
		body = jachoi::FileIO(path).read();
	}
	else// if (stat_rtn == -1)
	{
		jachoi::FileIO(path).write(req.body);

		std::string cgiresult = CGIStub(req.body).getCGIResult();

		res.setStatus(201);
		res.setContentType(".text/plain");
		res.setContentLocation(req.pathparser->path);
		body = "";
		//std::cout << "status code : 201" << std::endl;
	}
	res.makeRes(body);
}
