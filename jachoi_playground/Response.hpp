#pragma once
#include <iostream>
#include <string>
#include <map>

class Response
{
public:
	std::string servername;
	int statuscode;
	std::map<std::string, std::string> header;
private:
	std::string Status(int statuscode);
	std::string Date();
	std::string ContentType(const std::string& filename);
public:
	Response(const std::string& servername);
	const std::string& makeResFromText(int statuscode,	const std::string& content, bool chunked = false, const std::string& contenttype = "text/plain");
	const std::string& makeResFromFile(int statuscode, const std::string& filepath, bool chunked = false);
};
