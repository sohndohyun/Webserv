#include "Response.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "FileIO.hpp"
#include <sys/time.h>
Response::Response(const std::string& servername):servername(servername){}

std::string Response::Status(int statuscode)
{
	switch (statuscode)
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
		throw Exception("ResponseHeader: Invalid statusCode");
	}
}

std::string Response::Date()
{
	struct timeval curr;
	struct tm time;

	gettimeofday(&curr, NULL);
	strptime(std::to_string(curr.tv_sec).c_str(), "%s", &time);
	return jachoi::makeGMT(time.tm_zone, curr.tv_sec);
}

std::string Response::ContentType(const std::string& filename)
{
	std::string type = filename.substr(filename.find_last_of('.') + 1);

	if (type == "html" || type == "htm")
		return "text/html";
	else if (type == "css")
		return "text/css";
	else if (type == "js")
 		return "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		return "image/jpeg";
	return "text/plain";
}

std::string Response::makeResFromText(int statuscode, const std::string& content, bool chunked , const std::string& contenttype)
{
	std::string response;
// header
	response += "HTTP/1.1 " + std::to_string(statuscode) + " " + Status(statuscode) + "\r\n";
	response += "Date: " + Date() + "\r\n";
	response += "Server: " + servername  + "\r\n";
	response += "Content-Type: " + contenttype + "\r\n";
	if (chunked)
		response += "Transfer-Encoding: chunked\r\n";
	else
		response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
	response += "\r\n";
// body
	if (chunked)
		//TODO: Chunked Send
		(void)chunked;
	else
		response += content;
	return response;
}

std::string Response::makeResFromFile(int statuscode, const std::string& filepath, bool chunked)
{
	std::string response;
	std::string content = jachoi::FileIO(filepath).read();
// header
	response += "HTTP/1.1 " + std::to_string(statuscode) + " " + Status(statuscode) + "\r\n";
	response += "Date: " + Date() + "\r\n";
	response += "Server: " + servername  + "\r\n";
	response += "Content-Type: " + ContentType(filepath) + "\r\n";
	if (chunked)
		response += "Transfer-Encoding: chunked\r\n";
	else
		response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
	response += "\r\n";
// body
	if (chunked)
		//TODO: Chunked Send
		(void)chunked;
	else
		response += content;
	return response;

}


// const std::string& Response::makeRes(int status_code, ConfigParse::t_server *conf_server, std::string content_type, std::map<std::string, std::string> req_header, std::string content_length)
// {
// 	ResponseHeader res(status_code, conf_server, content_type, req_header, content_length);

// 	res_str = "HTTP/" + res.header["http"] + " " + res.header["status_code"] + " " + res.header["status_msg"] + "\r\n"
// 				+ "Date: " + res.header["Date"] + "\r\n"
// 				+ "Server: " + res.header["Server"] + "\r\n"
// 				+ "Content-Type: " + res.header["Content-Type"] + "\r\n";
// 	if (res.header.find("Content-Length") != res.header.end())
// 		res_str += "Content-Length: " + res.header["Content-Length"] + "\r\n";
// 	res_str += "\r\n";
// 	if (res.header["status_code"] != "201" && res.header["status_code"] != "204")
// 		res_str += body;
// 	return res_str;
// }
