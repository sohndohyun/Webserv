#include "Client.hpp"
#include <sys/time.h>
#include <iostream>

using namespace std;

Client::Client(int fd) : fd(fd), willDie(false),done(false), writtenCount(0), sent(false)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	lasttime = tv.tv_sec;
	response.clear();
	request.clear();
}

bool Client::isTimeout()
{
	struct timeval cur_tv;
	gettimeofday(&cur_tv, 0);

	if (cur_tv.tv_sec - lasttime > 300 && sent){
		cout << fd << " Time out" << endl;
		willDie = true;
	}
	return willDie;
}
