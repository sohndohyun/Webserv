#include "CGIStub.hpp"
#include "Exception.hpp"
#include "RequestParser.hpp"
#include "ChunkParser.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "Utils.hpp"
#include "FileIO.hpp"

#define debug

#include "errno.h"

// 환경변수를 설정해서 넘겨주는 것이 필요하다 
// 1. REQUEST_METHOD = GET , POST , PUT 기타 등등
// 2. SERVER_PROTOCOL = HTTP/1.1 을 넣어주면 되는것 같다
// 3. PATH_INFO = pathparser->path 를 넣어줌녀 된다
// 문제점은 이걸 어또케 std::string 으로 변환을 해줄까인데요!!!!

CGIStub::CGIStub(const std::string& req, const std::string& cgipath): cgipath(cgipath)
{
#ifdef debug
	using namespace std;
#endif
	pid_t pid = fork();
	if (pid == -1)
		throw Exception("cgi Fork error");
	if (pid == 0)
	{
		RequestParser r(req);
		std::string body;
		// char* const argv[] = {const_cast<char*>(cgipath.c_str()), 0};
		char *const argv[] = {const_cast<char*>("/usr/bin/cat"), 0};
		cerr << "setup envs" << endl;
		jachoi::set_env("REQUEST_METHOD", r.method);
		jachoi::set_env("SERVER_PROTOCOL", "HTTP/1.1");
		jachoi::set_env("PATH_INFO", r.pathparser->path);
		char ** envp = jachoi::get_envp();
		cerr << "chunking... " << endl;
		if (r.header["Transfer-Encoding"] == "chunked")
			body = ChunkParser(r.body).getData();
		else
			body = r.body;
		cerr << "writing..." << body.size() << endl;
		jachoi::FileIO(".tmp_cgi").write(body);
		dup2(open(".tmp_cgi", O_WRONLY), 1);
		// dup2(open(".cgi_result", O_CREAT | O_TRUNC | O_WRONLY, 0644), 1);
		cerr << "executing..." << cgipath << endl;
		cerr << cgipath << endl;
		cerr << argv[0] << endl;
		cerr <<argv[1] << endl;
		// execve(cgipath.c_str(), argv, envp);
		execve("/usr/bin/cat", argv, envp);
		cerr << "Exec failed : " << errno << endl;
		exit(1);
	}
	else
	{
		cout << "child pid : " << pid << endl;
		waitpid(pid, 0, 0);
		// result = jachoi::FileIO(".cgi_result").read();
	}
}

const std::string& CGIStub::getCGIResult()
{
	return result;
}

CGIStub::~CGIStub()
{
}
