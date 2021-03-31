#include "WebServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>

int main()
{
	using namespace std;

	WebServer webServer;
#if debug
	try
	{
#endif
		ConfigParse conf;
		webServer.run("0.0.0.0", vector<int>(1, conf.server->port));

#if debug
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
#endif
	return (0);
}

