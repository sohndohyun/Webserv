#include "EchoServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>
#include "Utils.hpp"

std::map<std::string, std::string> g_envp;


#if debug	
using namespace std;
#endif
int main(int argc, char** argv, char** envp)
{
	(void)argc;
	(void)argv;	
	g_envp = jachoi::make_envp(envp);
	EchoServer echoServer;
#if deubg
	try
	{
#endif
		ConfigParse conf;
		echoServer.run("0.0.0.0", std::vector<int>(1, conf.server->port));

#if debug
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
#endif
	return (0);
}

