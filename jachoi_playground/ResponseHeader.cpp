// #include "ResponseHeader.hpp"
// #include "Exception.hpp"
// // status code
// // body
// // server conf => global variable
// // chunked : bool
// ResponseHeader::ResponseHeader
// (int status_code, ConfigParse::t_server *conf_server, 
// std::string content_path, std::map<std::string, std::string> req_header, std::string content_length)
// {
// 	// initStatus();

// 	header["http"] = "1.1";
	
// 	setStatus(status_code);
// 	setDate();
// 	setServer(conf_server);
// 	setContentType(content_path);
// 	setContentLength(req_header, content_length);
// }

// void ResponseHeader::setStatus(int status_code)
// {
// 	if (status.find(std::to_string(status_code)) != status.end())
// 	{
// 		header["status_code"] = std::to_string(status_code);
// 		header["status_msg"] = this->status[std::to_string(status_code)];
// 	}
// 	else
// 		throw ResponseHeader::InvalidStatusCodeException();
// }

// void ResponseHeader::setDate()
// {
// 	struct timeval curr;
// 	struct tm time;

// 	gettimeofday(&curr, NULL);
// 	strptime(std::to_string(curr.tv_sec).c_str(), "%s", &time);
// 	header.insert(make_pair("Date", jachoi::makeGMT(time.tm_zone, curr.tv_sec)));
// }

// void ResponseHeader::setServer(ConfigParse::t_server *conf_server)
// {
// 	header.insert(make_pair("Server", conf_server->name));
// }

// void ResponseHeader::setContentType(std::string content_path)
// {
// 	std::string type = content_path.substr(content_path.find_last_of('.') + 1);

// 	if (type == "html" || type == "htm")
// 		header["Content-Type"] = "text/html";
// 	else if (type == "css")
// 		header["Content-Type"] = "text/css";
// 	else if (type == "js")
// 		header["Content-Type"] = "text/javascript";
// 	else if (type == "jpeg" || type == "jpg")
// 		header["Content-Type"] = "image/jpeg";
// 	else
// 		header["Content-Type"] = "text/plain";
// }

// void ResponseHeader::setContentLength(std::map<std::string, std::string> req_header, std::string content_length)
// {
// 	if (req_header.find("Transfer-Encoding")->second != "chunked")
// 		header.insert(make_pair("Content-Length", content_length));
// }

// std::string ResponseHeader::getStatus(int statuscode)
// {
// 	switch (statuscode)
// 	{
// 		case	100: return "Continue"; 
// 		case	101: return "Switching Protocols"; 
// 		case	103: return "Early Hints"; 
// 		case	200: return "OK"; 
// 		case	201: return "Created"; 
// 		case	202: return "Accepted";
// 		case	203: return "Non-Authoritativ Information"; 
// 		case	204: return "No Content"; 
// 		case	205: return "Reset Content";
// 		case	206: return "Partial Content";
// 		case	300: return "Multiple Choices";
// 		case	301: return "Moved Permanently";
// 		case	302: return "Found"; 
// 		case	303: return "See Other";
// 		case	304: return "Not Modified";
// 		case	307: return "Temporary Redirect"; 
// 		case	308: return "Permanent Redirect"; 
// 		case	400: return "Bad Request"; 
// 		case	401: return "Unauthorized";
// 		case	402: return "Payment Required";
// 		case	403: return "Forbidden";
// 		case	404: return "Not Found";
// 		case	405: return "Method Not Allowed";
// 		case	406: return "Not Acceptable";
// 		case	407: return "Proxy Authentication Required";
// 		case	408: return "Request Timeout";
// 		case	409: return "Conflict"; 
// 		case	410: return "Gone"; 
// 		case	411: return "Length Required";
// 		case	412: return "Precondition Failed";
// 		case	413: return "Payload Too Large";
// 		case	414: return "URI Too Long";
// 		case	415: return "Unsupported Media Type";
// 		case	416: return "Range Not Satisfiable";
// 		case	417: return "Expectation Failed";
// 		case	418: return "I'm a teapot";
// 		case	422: return "Unprocessable Entity";
// 		case	425: return "Too Early";
// 		case	426: return "Upgrade Required";
// 		case	428: return "Precondition Required";
// 		case	429: return "Too Many Requests";
// 		case	431: return "Request Header Fields Too Large";
// 		case	451: return "Unavailable For Legal Reasons";
// 		case	500: return "Internal Server Error";
// 		case	501: return "Not Implemented";
// 		case	502: return "Bad Gateway";
// 		case	503: return "Service Unavailable";
// 		case	504: return "Gateway Timeout";
// 		case	505: return "HTTP Version Not Supported";
// 		case	506: return "Variant Also Negotiates";
// 		case	507: return "Insufficient Storage";
// 		case	508: return "Loop Detected";
// 		case	510: return "Not Extended";
// 		case	511: return "Network Authentication Required";
// 	default:
// 		throw Exception("ResponseHeader: Invalid statusCode");
// 	}
// }
