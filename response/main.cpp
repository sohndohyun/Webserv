#include "Response.hpp"

int main()
{
	ConfigParse conf;
	Response res("1.1", "200", "OK", conf.server, "text/html; charset=utf-8", "1846");
	std::cout << "HTTP/" << res.header["http"] << " " << res.header["status_code"] << " " << res.header["status_msg"] << std::endl;
	std::cout << "Date: " << res.header["Date"] << std::endl;
	std::cout << "Server: " << res.header["Server"] << std::endl;
	std::cout << "Content-Type: " << res.header["Content-Type"] << std::endl;
	std::cout << "Content-Length: " << res.header["Content-Length"] << std::endl;
}
