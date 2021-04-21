#include "WebServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>

int main()
{
	try
	{
		ConfigParse cp;

		//for (int i = 0; i < (int)cp.conf.size(); i++)
		//{
		//	WebServer webServer(cp.conf[i]);
		//	//수정필요
		//	webServer.run("0.0.0.0", std::vector<int>(1, cp.conf[i].server.port[0]));
		//}
		WebServer webServer(cp.conf[0]);
		webServer.run("0.0.0.0", std::vector<int>(1, cp.conf[0].server.port[0]));

	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
