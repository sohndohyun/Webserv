#include "Client.hpp"

Client::Client(int fd) : fd(fd), done(false), willDie(false), writtenCount(0)
{
	response.clear();
	request.clear();
}
