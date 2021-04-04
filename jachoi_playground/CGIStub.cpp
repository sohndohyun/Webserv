#include "CGIStub.hpp"
#include "Exception.hpp"
#include "RequestParser.hpp"
#include "ChunkParser.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "Utils.hpp"
#define debug

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
	int pipes[2];
	if ((-1 == pipe(pipes)))
		throw Exception("cgi pipe error");
	pid_t pid = fork();
	if (pid == -1)
		throw Exception("cgi Fork error");
	if (pid == 0)
	{
		RequestParser r(req);
		std::string body;
		char* const argv[] = {const_cast<char*>(cgipath.c_str()), 0};

		cerr << "setup envs" << endl;
		jachoi::set_env("REQUEST_METHOD", r.method);
		jachoi::set_env("SERVER_PROTOCOL", "HTTP/1.1");
		jachoi::set_env("PATH_INFO", r.pathparser->path);
		char ** envp = jachoi::get_envp();
		cerr << "chunking... " << endl;
		dup2(pipes[1], 1);
		dup2(pipes[0], 0);
		if (r.header["Transfer-Encoding"] == "chunked")
			body = ChunkParser(r.body).getData();
		else
			body = r.body;
		cerr << "writing..." << body.size() << endl;
		const char *_body = body.c_str();
		int sz = static_cast<int>(body.size());
		write(1, _body, sz); //-> why so slow
		// write(1, "\x1a", 1); // eof??
		cerr << "executing..." << endl;
		execve(cgipath.c_str(), argv, envp);
		cerr << "Exec failed" << endl;
		exit(1);
	}
	else
	{
		cout << "child pid : " << pid << endl;
		waitpid(pid, 0, 0);
		close(pipes[1]);
		char buf[1];
		while (read(pipes[0], buf, 1) > 0)
			result += buf[0];
		close(pipes[0]);
	}
}

const std::string& CGIStub::getCGIResult()
{
	return result;
}

CGIStub::~CGIStub()
{
}
