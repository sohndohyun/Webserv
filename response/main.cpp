#include "Response.hpp"

int main()
{
	ConfigParse conf;
	std::string body = "<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>";
	std::string req_str = "GET / HTTP/1.1\r\n";
	RequestParser req(req_str);
	Response res(conf.server->name);
	res.setStatus(200);
	res.setContentType("text.html");
	res.makeRes(body, req.header.find("Transfer-Encoding")->second);
	std::cout << res.res_str << std::endl;

	std::cout << "--------------------" << std::endl;
	req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n";
	//req_str = "GET /index?name=value HTTP/1.1\r\nTransfer-Encoding: identity";
	RequestParser req2(req_str);
	res.setStatus(300);
	res.setContentType("image.jpg");
	res.makeRes(body, req2.header.find("Trasfer-Encoding")->second);
	std::cout << res.res_str << std::endl;
}
