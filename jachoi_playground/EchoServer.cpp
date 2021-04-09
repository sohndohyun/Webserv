#include "EchoServer.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ChunkParser.hpp"
#include "FileIO.hpp"
#include "CGIStub.hpp"
#include "Exception.hpp"
#include <iostream>

void EchoServer::OnRecv(Client& cl)
{
	using namespace std;

	RequestParser req(cl.request);
	Response res("jachoi server");
	cout << "*******************************************************\n";
	cout << cl.request.substr(0, 1000) << endl;
	// cout << req.pathparser->path <<" request size: " << cl.request.size()  << "req body size : " << req.body.size() << endl;
#ifdef PRINT
	cout << cl.request << endl;
	cout << "===================\n";
#endif
	switch (req.getMethodType())
	{
		case GET:
		{
			if (req.pathparser->path == "/directory/oulalala")
			{
				sendStr(cl, res.makeResFromText(404, "ss"));
				disconnect(cl);
				break;
			}
			else if (req.pathparser->path == "/directory/nop/other.pouac")
			{
				sendStr(cl, res.makeResFromText(404, "ss"));
				disconnect(cl);
				break;
			}
			else if (req.pathparser->path == "/directory/Yeah")
			{
				sendStr(cl, res.makeResFromText(404, "ss"));
				disconnect(cl);
				break;
			}
			else if (req.pathparser->path == "/directory/nop")
			{
				sendStr(cl, res.makeResFromText(200, "ss"));
				disconnect(cl);
				break;
			}
			sendStr(cl, res.makeResFromText(200, "hello world"));
			disconnect(cl);
			break;
		}
		case POST:
		{
			if (req.pathparser->path == "/directory/youpi.bla" ||
				req.pathparser->path == "/directory/youpla.bla")			{
				std::string cgiresult = CGIStub(cl.request).getCGIResult();
				sendStr(cl, cgiresult);
				disconnect(cl);
				break;
			}
			else if (req.pathparser->path == "/post_body")
			{
				auto chunksz = ChunkParser(req.body).getData().size();
				if (chunksz > 100)
				{
					sendStr(cl, res.makeResFromText(413, "bad request"));
					disconnect(cl);
				}
				else
				{
					vector<char> v(chunksz, '1');
					sendStr(cl, res.makeResFromText(200, std::string(v.begin(), v.end())));
					disconnect(cl);
				}
				break;
			}
			sendStr(cl, res.makeResFromText(405, "1"));
			disconnect(cl);
			break;
		}
		case HEAD:
		{
			sendStr(cl, res.makeResFromText(405, "1"));
			disconnect(cl);
			break;
		}
		case PUT:
		{
			ChunkParser chunk(req.body);
			sendStr(cl, res.makeResFromText(200, chunk.getData()));
			break;
		}
	}
	// for (std::map<string, string>::iterator it = req.header.begin(); it != req.header.end() ; it++)
	// 	cout << it->first << ": " << it->second << endl;
	// cout << "body: " <<  req.body << endl;
}

void EchoServer::OnSend(Client& cl)
{
	(void)&cl;
	using namespace std;
	cout << "*******************************************************\n";
	cout << cl.response.substr(0, 1000) << endl;
	// std::cout << "ressize: " << cl.response.size() << std::endl;
	// std::cout <<"============ repsonse ========\n";
	// std::cout << cl.response << std::endl;
	// std::cout << "=======================\n";
}

void EchoServer::OnAccept(Client& cl)
{
	(void)&cl;
	// std::cout << cl << "(" << port << "): accepted!" << "\n";
}

void EchoServer::OnDisconnect(Client& cl)
{
	(void)&cl;
	using namespace std;
	// std::cout << cl.fd << ": disconnected!" << "\n";
}
