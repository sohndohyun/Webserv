#include "EchoServer.hpp"
#include <iostream>
int main()
{
	EchoServer echoServer;
	std::vector<int> a;
	a.push_back(3000);
	try
	{
		echoServer.run("0.0.0.0", a);
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}

