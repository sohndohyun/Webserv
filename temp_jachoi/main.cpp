#include "include/WebServer.hpp"
#include "include/ConfigParse.hpp"
#include <iostream>

int main()
{
	WebServer webServer;
	try
	{
		ConfigParse conf;
		webServer.run("0.0.0.0", std::vector<int>(1, conf.server->port));
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
