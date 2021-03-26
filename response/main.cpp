#include "ResponseHeader.hpp"
#include "Response.hpp"
#include "../parser/RequestParser.hpp"

int main()
{
	ConfigParse conf;
	std::string body = "<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>";
	std::string req_str = "GET / HTTP/1.1\r\n";
	RequestParser req2(req_str);
	Response res2(body);
	res2.makeRes(200, conf.server, "text.html", req2.header, std::to_string(body.length()));
	std::cout << res2.res_str << std::endl;

	std::cout << "--------------------" << std::endl;
	req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n";
	//req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: identity";
	RequestParser req(req_str);
	Response res(body);
	res.makeRes(300, conf.server, "image", req.header, std::to_string(body.length()));
	std::cout << res.res_str << std::endl;
}
