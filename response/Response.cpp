#include "Response.hpp"

Response::Response(std::string status_code, std::string status_msg, ConfigParse::t_server *conf_server, std::string content_path, std::map<std::string, std::string> req_header, std::string content_length)
{
	header["http"] = "1.1";
	header.insert(make_pair("status_code", status_code));
	header.insert(make_pair("status_msg", status_msg));

	setDate();
	setServer(conf_server);
	setContentType(content_path);
	setContentLength(req_header, content_length);
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

void Response::setContentType(std::string content_path)
{
	std::string type = content_path.substr(content_path.find('.') + 1);

	if (type == "html" || type == "htm")
		header["Content-Type"] = "text/html";
	else if (type == "css")
		header["Content-Type"] = "text/css";
	else if (type == "js")
		header["Content-Type"] = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		header["Content-Type"] = "image/jpeg";
	else
		header["Content-Type"] = "text/plain";
	
	//https://developer.mozilla.org/ko/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
	//else if (type == "png")
	//	header["Content-Type"] = "png";
	//else if (type == "bmp")
	//	header["Content-Type"] = "bmp";
}

void Response::setContentLength(std::map<std::string, std::string> req_header, std::string content_length)
{
	if (req_header.find("Transfer-Encoding")->second != "chunked")
		header.insert(make_pair("Content-Length", content_length));
}