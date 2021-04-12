#pragma once
#include <string>

class Client
{
public:
	int fd;
	bool willDie;
	bool done;
	size_t writtenCount;
	bool sent;
	int lasttime;
	std::string response;
	std::string request;
public:
	Client(int fd);
	bool isTimeout();
};
