#pragma once

#include "AServer.hpp"
#include "Request.hpp"
#include <map>

#ifdef __linux__
#define CGI_PATH "./ubuntu_cgi_tester"
#else
#define CGI_PATH "./cgi_tester"
#endif

class ConfigParse;
class Response;

class WebServer : public AServer
{
private:
	class FileData
	{
	public:
		FileData(int fd, Response *res, bool isCGI = false, char **envp = NULL, std::string const &path = "");
		~FileData();
		int fd;
		Response *res;
		bool isCGI;
		char **envp;
		std::string path;
	};

	std::map<int, Request*> requests;

	std::map<int, std::string> reqStr;
	ConfigParse &conf;

public:
	WebServer(ConfigParse &conf);

	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);

	virtual void OnFileRead(int fd, std::string const &str, void *temp);
	virtual void OnFileWrite(int fd, void *temp);

	virtual ~WebServer();

	void request_process(int fd, Request &req);
	int cgi_stub(int tempfd, FileData *fData);

	void methodGET(int fd, Response *res, std::string req_path);
	void methodHEAD(int fd, Response *res, std::string req_path);
	void methodPUT(int fd, Response *res, Request &req);
	void methodPOST(int fd, Response *res, Request &req);
	void methodInvalid(int fd, Response *res, Request &req);
};
