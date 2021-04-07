#include "EchoServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ChunkParser.hpp"
#include "FileIO.hpp"
#include "CGIStub.hpp"
#include "Exception.hpp"
#include <iostream>

bool EchoServer::OnRecv(int fd, std::string const &str)
{
	using namespace std;
	(void)fd;


	try{
	RequestParser req(str);
	cout << "-------str " << str.size() <<  "----------\n";
	cout << str.substr(0,500) << endl;
	cout << "===================\n";
	Response res("jachoi server");
	switch (req.getMethodType())
	{
		case GET:
		{
			cout << "=path=====================================\n";
			cout <<  req.pathparser->path << endl;
			cout << "==========================================\n";
			if (req.pathparser->path == "/directory/oulalala")
			{
				sendStr(fd, res.makeResFromText(404, "ss"));
				disconnect(fd);
				return true;
			}
			else if (req.pathparser->path == "/directory/nop/other.pouac")
			{
				sendStr(fd, res.makeResFromText(404, "ss"));
				disconnect(fd);
				return true;
			}
			else if (req.pathparser->path == "/directory/Yeah")
			{
				sendStr(fd, res.makeResFromText(404, "ss"));
				disconnect(fd);
				return true;
			}
			sendStr(fd, res.makeResFromText(200, "hello world"));
			disconnect(fd);
			break;
		}
		case POST:
		{
			if (req.pathparser->path == "/directory/youpi.bla" ||
				req.pathparser->path == "/directory/youpla.bla")			{
				jachoi::FileIO("request").write(str);
				cout << "Recved post data" << str.size() << endl;
				std::string cgiresult = CGIStub(str).getCGIResult();
				cout <<"==================" << endl;
				cout << "cgi : " << cgiresult.substr(0, 2000) << endl;
				sendStr(fd, cgiresult);
				disconnect(fd);
				return true;
			}
			else if (req.pathparser->path == "/post_body")
			{
				cerr << "chunking ... post body" << endl;
				auto chunksz = ChunkParser(req.body).getData().size();
				if (chunksz > 100)
					sendStr(fd, res.makeResFromText(413, "bad request"));
				else
				{
					vector<char> v(chunksz, '1');
					sendStr(fd, res.makeResFromText(200, std::string(v.begin(), v.end())));
					disconnect(fd);
				}
				return true;
			}
			sendStr(fd, res.makeResFromText(405, "1"));
			disconnect(fd);
			break;
		}
		case HEAD:
		{
			sendStr(fd, res.makeResFromText(405, "1"));
			disconnect(fd);
			break;
		}
		case PUT:
		{
			ChunkParser chunk(req.body);
			cout << chunk.getData().size() << endl;
			// cout << chunk.getData() << endl;
			sendStr(fd, res.makeResFromText(200, chunk.getData()));
		}
		return true;
	}
	// for (std::map<string, string>::iterator it = req.header.begin(); it != req.header.end() ; it++)
	// 	cout << it->first << ": " << it->second << endl;
	// cout << "body: " <<  req.body << endl;
	}
	catch(...)
	{
		return false;
	}
}

void EchoServer::OnSend(int fd)
{
	(void)&fd;
}

void EchoServer::OnAccept(int fd, int port)
{
	(void)&fd;
	(void)&port;
	// std::cout << fd << "(" << port << "): accepted!" << "\n";
}

void EchoServer::OnDisconnect(int fd)
{
	(void)&fd;
	// std::cout << fd << ": disconnected!" << "\n";ㄴ
}
