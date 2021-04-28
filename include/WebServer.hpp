#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
#include "AServer.hpp"
#include "Request.hpp"
#include <map>

#ifdef __linux__
#define CGI_PATH "./ubuntu_cgi_tester"
#else
#define CGI_PATH "./cgi_tester"
#endif

#include "ConfigParse.hpp"
class Response;

class WebServer : public AServer
{
private:
	class FileData
	{
	public:
		FileData(int fd, Response *res, bool isCGI = false, char **envp = NULL, std::string const &path = "", int methodtype = POST);
		~FileData();
		int fd;
		Response *res;
		bool isCGI;
		char **envp;
		std::string path;
		int methodtype;
	};

	std::map<int, Request*> requests;

	std::map<int, std::string> reqStr;
	ConfigParse &confs;

public:
	AServer::t_analysis analysis;

public:
	WebServer(ConfigParse &conf);

	virtual void OnRecv(int fd, int port, std::string const &str);
	virtual void OnSend(int fd, int port);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd, int port);

	virtual void OnFileRead(int fd, std::string const &str, void *temp);
	virtual void OnFileWrite(int fd, void *temp);

	virtual ~WebServer();

	void request_process(int fd, int port, Request &req);
	int cgi_stub(int tempfd, FileData *fData);


	void methodGET(int fd, int port, Response *res, Request &req);
	void methodHEAD(int fd, int port, Response *res, Request &req);
	void methodPUT(int fd, int port, Response *res, Request &req);
	void methodPOST(int fd, int port, Response *res, Request &req);

private:
	int get_conf_idx(int port);
	void errorRes(int fd, int port, Response *res, int errorCode, const std::vector<std::string>& allow_methods = std::vector<std::string>());
};
#endif
