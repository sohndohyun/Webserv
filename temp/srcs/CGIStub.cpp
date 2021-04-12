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
#endif
CGIStub::CGIStub(RequestParser const &req, std::string path, const std::string& cgipath): cgipath(cgipath)
{

	int wpipe[2];
	int rpipe[2];
	std::map<std::string, std::string> req_header = req.header;
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
			jachoi::set_env("REQUEST_METHOD", req.method);
			jachoi::set_env("SERVER_PROTOCOL", "HTTP/1.1");
			jachoi::set_env("PATH_INFO", path);
				//TODO  Utils 에 HTTP_로 변환하는거 만들기
			if (req_header.find("X-Secret-Header-For-Test") != req_header.end())
			{
				jachoi::set_env("HTTP_X_SECRET_HEADER_FOR_TEST", req_header["X-Secret-Header-For-Test"]);
			}
			char **envp = jachoi::get_envp();

			// std::cerr << "writing..." << req.body.size() << std::endl;
			// jachoi::FileIO(".tmp_cgi").write(body);
			dup2(rpipe[0], 0);
			dup2(wpipe[1], 1);
			execve(argv[0], argv, envp);
			std::cerr << "Exec failed : " << errno << std::endl;
			exit(1);
		}
		default: // 부모 프로세스
		{
			char bufs[1000000] = {0};
			int rdbytes = -1;
			int wrbytes = 0;
			while (true)
			{
				if (wrbytes == req.body.size())
					break;
				if (wrbytes + 30000 < req.body.size())
					wrbytes += write(rpipe[1], req.body.c_str() + wrbytes, 30000);
				else if (req.body.size() - wrbytes > 0)
					wrbytes += write(rpipe[1], req.body.c_str() + wrbytes, req.body.size() - wrbytes);
				rdbytes = read(wpipe[0], bufs, sizeof(bufs));
				result.append(std::string(bufs, 0, rdbytes));
			}
			close(wpipe[0]);
			close(wpipe[1]);
			close(rpipe[0]);
			close(rpipe[1]);
			kill(pid, 9);
			std::cout << "======cgi result======" << std::endl;
			std::cout << "result size : " << result.size() << std::endl;
			std::cout << result.substr(0, 100) << std::endl;
			std::cout << "======cgi result======" << std::endl;
		}
	}
}

const std::string& CGIStub::getCGIResult()
{
	int idx = result.length();
	if (result.find("Status") != std::string::npos)
		idx = result.find("Status");
	if (result.find("Content-Type") != std::string::npos && idx > result.find("Content-Type"))
		idx = result.find("Content-Type");
	if (idx != result.length())
		result = result.erase(idx, result.find("\r\n\r\n") - idx + 4);

	return result;
}

CGIStub::~CGIStub()
{
}
