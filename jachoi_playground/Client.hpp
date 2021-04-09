#pragma once
#include <string>

class Client
{
public:
	int fd;
	bool willDie;
	bool done;
	int sendCount;
	std::string response;
	std::string request;
public:
	Client(int fd);
};
