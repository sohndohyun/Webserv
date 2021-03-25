#include "Response.hpp"

int main()
{
	ConfigParse conf;
	std::string req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: chunked";
	//req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: identity";
	//req_str = "GET / HTTP/1.1\r\n";
	RequestParser req(req_str);
	Response res("200", "OK", conf.server, "text.html", req.header, "123");
	std::cout << "HTTP/" << res.header["http"] << " " << res.header["status_code"] << " " << res.header["status_msg"] << std::endl;
	std::cout << "Date: " << res.header["Date"] << std::endl;
	std::cout << "Server: " << res.header["Server"] << std::endl;
	std::cout << "Content-Type: " << res.header["Content-Type"] << std::endl;
	std::cout << "Content-Length: " << res.header["Content-Length"] << std::endl;
}
