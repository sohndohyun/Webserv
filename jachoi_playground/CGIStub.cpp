#include "CGIStub.hpp"
#include "Exception.hpp"
#include "RequestParser.hpp"
#include "ChunkParser.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "Utils.hpp"
#include "FileIO.hpp"
#include <signal.h>


#define debug
#ifdef debug
	#include "errno.h"
	using namespace std;
#endif
CGIStub::CGIStub(const std::string& req, const std::string& cgipath): cgipath(cgipath)
{

	int wpipe[2];
	int rpipe[2];
	RequestParser r(req);
	if (pipe(wpipe) == -1 || pipe(rpipe) == -1)
		throw Exception("pipe error");
	pid_t pid = fork();
	switch (pid)
	{
		case -1:
		{
			throw Exception("cgi Fork error");
			break;
		}
		case 0: // 자식 프로세스
		{
			char* const argv[] = {const_cast<char*>(cgipath.c_str()), 0};
			jachoi::set_env("REQUEST_METHOD", r.method);
			jachoi::set_env("SERVER_PROTOCOL", "HTTP/1.1");
			jachoi::set_env("PATH_INFO", r.pathparser->path);
				//TODO  Utils 에 HTTP_로 변환하는거 만들기
			if (r.header.find("X-Secret-Header-For-Test") != r.header.end())
			{
				jachoi::set_env("HTTP_X_SECRET_HEADER_FOR_TEST", r.header["X-Secret-Header-For-Test"]);
			}
			char ** envp = jachoi::get_envp();

			// cerr << "writing..." << body.size() << endl;
			// jachoi::FileIO(".tmp_cgi").write(body);
			dup2(rpipe[0], 0);
			dup2(wpipe[1], 1);
			execve(argv[0], argv, envp);
			cerr << "Exec failed : " << errno << endl;
			exit(1);
		}
		default: // 부모 프로세스
		{
			char bufs[1000000] = {0};
			std::string body =  r.header["Transfer-Encoding"] == "chunked" ?
				ChunkParser(r.body).getData() : r.body;
			result.append("HTTP/1.1 200 OK\r\n");
			result.append("Content-Type: text/html; charset=utf-8\r\n");
			cout << "pipe start " << endl;
			int rdbytes = -1;
			int wrbytes = 0;
			while (true)
			{
				if (wrbytes == body.size())
					break;
				cout << "wrbytes : " << wrbytes << "rdbytes : " << rdbytes  << endl;
				if (wrbytes + 30000 < body.size())
					wrbytes += write(rpipe[1], body.c_str() + wrbytes, 30000);
				else if (body.size() - wrbytes > 0)
					wrbytes += write(rpipe[1], body.c_str() + wrbytes, body.size() - wrbytes);
				rdbytes = read(wpipe[0], bufs, sizeof(bufs));
				result.append(std::string(bufs, 0, rdbytes));
			}
			cout << result.size() << endl;
			cout << result.substr(0, 100) << endl;
			cout << "pipe end" << endl;
			close(wpipe[0]);
			close(wpipe[1]);
			close(rpipe[0]);
			close(rpipe[1]);
			kill(pid, 9);
			cout << "result : " <<  result.substr(0, 100) << endl;
		}
	}
}

const std::string& CGIStub::getCGIResult()
{
	return result;
}

CGIStub::~CGIStub()
{
}
