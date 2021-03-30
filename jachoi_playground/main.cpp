#include "EchoServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>

int main()
{
	using namespace std;

	EchoServer echoServer;
#if deubg
	try
	{
#endif
		ConfigParse conf;
		echoServer.run("0.0.0.0", vector<int>(1, conf.server->port));

#if debug
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
#endif
	return (0);
}

