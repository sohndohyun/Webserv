#include "include/WebServer.hpp"
#include "include/ConfigParse.hpp"
#include <iostream>

int main()
{
	using namespace std;

	try
	{
		ConfigParse conf;
		WebServer webServer(conf);
		webServer.run("0.0.0.0", vector<int>(1, conf.server->port));
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
