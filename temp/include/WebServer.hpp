#ifndef webserver_hpp
#define webserver_hpp
#include "AServer.hpp"

class RequestParser;
class ConfigParse;
class Response;

class WebServer : public AServer
{
private:
	std::string reqStr;
	ConfigParse &conf;

public:
	WebServer(ConfigParse &conf);

	virtual void OnRecv(int fd, std::string const &str);
	virtual void OnSend(int fd);
	virtual void OnAccept(int fd, int port);
	virtual void OnDisconnect(int fd);

	void request_process(int fd, RequestParser const &req);
	void methodGET(Response &res, std::string req_path);
	void methodHEAD(Response &res, std::string req_path);
	void methodPUT(Response &res, RequestParser const &req);
	void methodPOST(Response &res, RequestParser const &req);

};

#endif
