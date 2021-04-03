#include "CGIStub.hpp"
#include "Exception.hpp"
#include "RequestParser.hpp"
#include "ChunkParser.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

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

	cout << "cgi stub started" << endl;
	pid_t pid = fork();
	if (pid == -1)
		throw Exception("cgi Fork error");
	if (pid == 0)
	{
		dup2(pipes[1], 1);
		dup2(pipes[0], 0);
		cerr << "dup2 " << endl;
		RequestParser r(req);
		std::string body = r.body;
		if (r.header["Transfer-Encoding"] == "chunked")
		{
			cerr << "chunked 에욤" << endl;
			body = ChunkParser(r.body).getData();
		}
		cerr << "body size : " << body.size() << endl;
		cerr << body.substr(0 , 100 ) << endl;
		write(1, body.c_str(), body.size());
		// cout << body.c_str();
		cerr << "write 함" << endl;
	//argv make
		char* const argv[] = {const_cast<char*>(cgipath.c_str()), 0};

	//envp make
		std::string req_method = "REQUEST_METHOD=" + r.method;
		std::string serv_protocol = "SERVER_PROTOCOL=HTTP/1.1";
		std::string path_info ="PATH_INFO=" + r.pathparser->path;
		char* const envp[] = {const_cast<char*>(req_method.c_str()), const_cast<char*>(serv_protocol.c_str()), const_cast<char*>(path_info.c_str()), 0};
		cerr << "Execve 시작" << endl;
		execve(cgipath.c_str(), argv, envp);
		throw Exception("Invalid cgi executables");
	}
	else
	{
		waitpid(pid, 0, 0);
		cerr << "대기 완료" << endl;
		close(pipes[1]);
		char buf[1];
		while (read(pipes[0], buf, 1) > 0)
			result += buf[0];
		close(pipes[0]);
#ifdef debug
		using namespace std;
		cout << "result size : " << result.size() << endl;
#endif
	}
}

const std::string& CGIStub::getCGIResult()
{
	return result;
}

CGIStub::~CGIStub()
{
}
