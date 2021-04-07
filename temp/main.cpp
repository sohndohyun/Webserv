#include "include/WebServer.hpp"
#include "include/ConfigParse.hpp"
#include <iostream>
#include "include/Utils.hpp"

std::map<std::string, std::string> g_envp;

int main(int argc, char** argv, char** envp)
{
	(void)argc;
	(void)argv;	
	g_envp = jachoi::make_envp(envp);
	try
	{
		ConfigParse conf;
		WebServer webServer(conf);
		webServer.run("0.0.0.0", std::vector<int>(1, conf.server->port));
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
