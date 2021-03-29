#include "EchoServer.hpp"
#include "ConfigParse.hpp"
#include <iostream>
int main()
{
	using namespace std;
	
	EchoServer echoServer;
	try
	{
		ConfigParse conf;	
		echoServer.run("0.0.0.0", {3000});
	}
	catch(const std::exception& e) 
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}

