#include "Response.hpp"

Response::Response(std::string body):
	body(body)
{
}

Response::~Response()
{
}

void Response::makeRes(int status_code, ConfigParse::t_server *conf_server, std::string content_type, std::map<std::string, std::string> req_header, std::string content_length)
{
	ResponseHeader res(status_code, conf_server, content_type, req_header, content_length);

	res_str = "HTTP/" + res.header["http"] + " " + res.header["status_code"] + " " + res.header["status_msg"] + "\r\n"
				+ "Date: " + res.header["Date"] + "\r\n"
				+ "Server: " + res.header["Server"] + "\r\n"
				+ "Content-Type: " + res.header["Content-Type"] + "\r\n";
	if (res.header.find("Content-Length") != res.header.end())
		res_str += "Content-Length: " + res.header["Content-Length"] + "\r\n";
	res_str += "\r\n";
	if (res.header["status_code"] != "201" && res.header["status_code"] != "204")
		res_str += body;
}
