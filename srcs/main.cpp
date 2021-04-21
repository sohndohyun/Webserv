#include "WebServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>
#include "Utils.hpp"

int main()
{
	try
	{
		ConfigParse cp;
		WebServer webServer(cp);
		webServer.run("0.0.0.0", cp.port);
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
