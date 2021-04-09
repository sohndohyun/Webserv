#include "Client.hpp"

Client::Client(int fd) : fd(fd), done(false), willDie(false), sendCount(1)
{
	response.clear();
	request.clear();
}
