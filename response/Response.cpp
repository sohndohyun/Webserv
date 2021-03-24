#include "Response.hpp"

Response::Response(std::string http_v, std::string status_code, std::string status_msg, ConfigParse::t_server *conf_server, std::string content_type, std::string content_length)
{
	header.insert(make_pair("http", http_v));
	header.insert(make_pair("status_code", status_code));
	header.insert(make_pair("status_msg", status_msg));

	setDate();
	setServer(conf_server);
	setContentType(content_type);
	setContentLength(content_length);
}

Response::~Response() {};

void Response::setDate()
{
	struct timeval curr;
	struct tm time;

	gettimeofday(&curr, NULL);
	strptime(std::to_string(curr.tv_sec).c_str(), "%s", &time);
	header.insert(make_pair("Date", ft_makeGMT(time.tm_zone, curr.tv_sec)));
}

void Response::setServer(ConfigParse::t_server *conf_server)
{
	header.insert(make_pair("Server", conf_server->name));
}

void Response::setContentType(std::string content_type)
{
	header.insert(make_pair("Content-Type", content_type));
}

void Response::setContentLength(std::string content_length)
{
	header.insert(make_pair("Content-Length", content_length));

	//https://developer.mozilla.org/ko/docs/Web/HTTP/Headers/Content-Type
	//Content-Type: text/html; charset=utf-8
	//Content-Type: multipart/form-data; boundary=something
}
