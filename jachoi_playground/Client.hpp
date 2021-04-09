#pragma once
#include <string>

class Client
{
public:
	int fd;
	bool willDie;
	bool done;
	int writtenCount;
	std::string response;
	std::string request;
public:
	Client(int fd);
};
