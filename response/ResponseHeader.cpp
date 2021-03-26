#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(int status_code, ConfigParse::t_server *conf_server, std::string content_path, std::map<std::string, std::string> req_header, std::string content_length)
{
	initStatus();
	header["http"] = "1.1";
	setStatus(status_code);
	setDate();
	setServer(conf_server);
	setContentType(content_path);
	setContentLength(req_header, content_length);
}

ResponseHeader::~ResponseHeader() {}

const char *ResponseHeader::InvalidStatusCodeException::what() const throw()
{
	return ("Exception : Invalid status code");
}

void ResponseHeader::setStatus(int status_code)
{
	if (status.find(std::to_string(status_code)) != status.end())
	{
		header["status_code"] = std::to_string(status_code);
		header["status_msg"] = this->status[std::to_string(status_code)];
	}
	else
		throw ResponseHeader::InvalidStatusCodeException();
}

void ResponseHeader::setDate()
{
	struct timeval curr;
	struct tm time;

	gettimeofday(&curr, NULL);
	strptime(std::to_string(curr.tv_sec).c_str(), "%s", &time);
	header.insert(make_pair("Date", ft_makeGMT(time.tm_zone, curr.tv_sec)));
}

void ResponseHeader::setServer(ConfigParse::t_server *conf_server)
{
	header.insert(make_pair("Server", conf_server->name));
}

void ResponseHeader::setContentType(std::string content_path)
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

void ResponseHeader::setContentLength(std::map<std::string, std::string> req_header, std::string content_length)
{
	if (req_header.find("Transfer-Encoding")->second != "chunked")
		header.insert(make_pair("Content-Length", content_length));
}

void ResponseHeader::initStatus()
{
	status["100"] = "Continue";
	status["101"] = "Switching Protocols";
	status["103"] = "Early Hints";
	status["200"] = "OK";
	status["201"] = "Created";
	status["202"] = "Accepted";
	status["203"] = "Non-Authoritativ Information";
	status["204"] = "No Content";
	status["205"] = "Reset Content";
	status["206"] = "Partial Content";
	status["300"] = "Multiple Choices";
	status["301"] = "Moved Permanently";
	status["302"] = "Found";
	status["303"] = "See Other";
	status["304"] = "Not Modified";
	status["307"] = "Temporary Redirect";
	status["308"] = "Permanent Redirect";
	status["400"] = "Bad Request";
	status["401"] = "Unauthorized";
	status["402"] = "Payment Required";
	status["403"] = "Forbidden";
	status["404"] = "Not Found";
	status["405"] = "Method Not Allowed";
	status["406"] = "Not Acceptable";
	status["407"] = "Proxy Authentication Required";
	status["408"] = "Request Timeout";
	status["409"] = "Conflict";
	status["410"] = "Gone";
	status["411"] = "Length Required";
	status["412"] = "Precondition Failed";
	status["413"] = "Payload Too Large";
	status["414"] = "URI Too Long";
	status["415"] = "Unsupported Media Type";
	status["416"] = "Range Not Satisfiable";
	status["417"] = "Expectation Failed";
	status["418"] = "I'm a teapot";
	status["422"] = "Unprocessable Entity";
	status["425"] = "Too Early";
	status["426"] = "Upgrade Required";
	status["428"] = "Precondition Required";
	status["429"] = "Too Many Requests";
	status["431"] = "Request Header Fields Too Large";
	status["451"] = "Unavailable For Legal Reasons";
	status["500"] = "Internal Server Error";
	status["501"] = "Not Implemented";
	status["502"] = "Bad Gateway";
	status["503"] = "Service Unavailable";
	status["504"] = "Gateway Timeout";
	status["505"] = "HTTP Version Not Supported";
	status["506"] = "Variant Also Negotiates";
	status["507"] = "Insufficient Storage";
	status["508"] = "Loop Detected";
	status["510"] = "Not Extended";
	status["511"] = "Network Authentication Required";
}
