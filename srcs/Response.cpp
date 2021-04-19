#include "Response.hpp"
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include "Utils.hpp"
#include "ConfigParse.hpp"
#include "Exception.hpp"

Response::Response(std::string server_name)
{
	header.insert(make_pair("Server", server_name));
}

Response::~Response() {}

std::string Response::initStatus(int status_code)
{
	switch (status_code)
	{
		case	100: return "Continue";
		case	101: return "Switching Protocols";
		case	103: return "Early Hints";
		case	200: return "OK";
		case	201: return "Created";
		case	202: return "Accepted";
		case	203: return "Non-Authoritativ Information";
		case	204: return "No Content";
		case	205: return "Reset Content";
		case	206: return "Partial Content";
		case	300: return "Multiple Choices";
		case	301: return "Moved Permanently";
		case	302: return "Found";
		case	303: return "See Other";
		case	304: return "Not Modified";
		case	307: return "Temporary Redirect";
		case	308: return "Permanent Redirect";
		case	400: return "Bad Request";
		case	401: return "Unauthorized";
		case	402: return "Payment Required";
		case	403: return "Forbidden";
		case	404: return "Not Found";
		case	405: return "Method Not Allowed";
		case	406: return "Not Acceptable";
		case	407: return "Proxy Authentication Required";
		case	408: return "Request Timeout";
		case	409: return "Conflict";
		case	410: return "Gone";
		case	411: return "Length Required";
		case	412: return "Precondition Failed";
		case	413: return "Payload Too Large";
		case	414: return "URI Too Long";
		case	415: return "Unsupported Media Type";
		case	416: return "Range Not Satisfiable";
		case	417: return "Expectation Failed";
		case	418: return "I'm a teapot";
		case	422: return "Unprocessable Entity";
		case	425: return "Too Early";
		case	426: return "Upgrade Required";
		case	428: return "Precondition Required";
		case	429: return "Too Many Requests";
		case	431: return "Request Header Fields Too Large";
		case	451: return "Unavailable For Legal Reasons";
		case	500: return "Internal Server Error";
		case	501: return "Not Implemented";
		case	502: return "Bad Gateway";
		case	503: return "Service Unavailable";
		case	504: return "Gateway Timeout";
		case	505: return "HTTP Version Not Supported";
		case	506: return "Variant Also Negotiates";
		case	507: return "Insufficient Storage";
		case	508: return "Loop Detected";
		case	510: return "Not Extended";
		case	511: return "Network Authentication Required";
	default:
		throw Exception("Response: Invalid status code");
	}
}

void Response::setDate()
{
	struct timeval curr;
	struct tm time;

	gettimeofday(&curr, NULL);
	strptime(jachoi::to_string(curr.tv_sec).c_str(), "%s", &time);
	header.insert(make_pair("Date", jachoi::makeGMT(time.tm_zone, curr.tv_sec)));
}

void Response::setStatus(int status_code)
{
	header["status_msg"] = initStatus(status_code);
	header["status_code"] = jachoi::to_string(status_code);
}

void Response::setContentType(std::string content_path)
{
	std::string type = content_path.substr(content_path.rfind('.') + 1);

	if (type == "html" || type == "htm")
		header["Content-Type"] = "text/html";
	else if (type == "css")
		header["Content-Type"] = "text/css";
	else if (type == "js")
		header["Content-Type"] = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		header["Content-Type"] = "image/jpeg";
	else if (type == "bla")
		header["Content-Type"] = "text/html; charset=utf-8";
	else
		header["Content-Type"] = "text/plain";
}

void Response::setContentLocation(std::string req_path)
{
	header["Content-Location"] = req_path;
}

void Response::setAllow(std::vector<std::string> allowMethods)
{
	std::string str;

	std::vector<std::string>::iterator iter = allowMethods.begin();
	for(; iter != allowMethods.end(); iter++)
	{
		str += *iter;
		if (iter + 1 != allowMethods.end())
			str += ", ";
	}
	header["Allow"] = str;
}

void Response::setLocation(std::string req_path)
{
	header["Location"] = req_path;
}

void Response::setLastModified(std::string content_path)
{
	struct stat sb;
	struct tm time;

	stat(content_path.c_str(), &sb);

	strptime(ctime(&sb.st_mtime), "%s", &time);
	header["Last-Modified"] = jachoi::makeGMT(time.tm_zone, sb.st_mtime);
}

void Response::setRetryAfter(void)
{
	struct timeval curr;
	struct tm time;

	gettimeofday(&curr, NULL);
	curr.tv_sec += 365 * 24 * 60 * 60;
	strptime(jachoi::to_string(curr.tv_sec).c_str(), "%s", &time);
	header["Retry-After"] = jachoi::makeGMT(time.tm_zone, curr.tv_sec);
}

void Response::setWWWAuthenticate(void)
{
	header["WWW-Authenticate"] = "Basic realm=\"Access to the staging site or folder.\"";
}


void Response::makeRes(std::string body, bool isPUT, bool chunked)
{
	setDate();
	res_str = "HTTP/1.1 " + header["status_code"] + " " + header["status_msg"] + "\r\n";
	res_str += "Content-Language: ko-KR\r\n";

	std::map<std::string, std::string>::iterator iter = header.begin();
	for(; iter != header.end(); iter++)
	{
		if (iter->first != "status_code" && iter->first != "status_msg")
			res_str += iter->first + ": " + iter->second + "\r\n";
	}
	if (chunked == false)
		res_str += "Content-Length: " + jachoi::to_string(body.length()) + "\r\n";
	else if (isPUT == false)
		res_str += "Transfer-Encoding: chunked\r\n";

	res_str += "\r\n";
	if (header["status_code"] != "201" && header["status_code"] != "204")
		res_str += body;
}
