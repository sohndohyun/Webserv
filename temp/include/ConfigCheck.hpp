#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <string>
#include "ConfigParse.hpp"
class RequestParser;

class ConfigCheck
{
private:
	ConfigParse &conf;
	std::string req_path;

private:
	std::string findLocation();
	std::string getRootURL();
	std::string makeAutoIdx(std::string path);

public:
	ConfigCheck(ConfigParse &conf, std::string req_path);
	~ConfigCheck();

	std::string makeFilePath();
	std::string findPath();
	bool methodCheck(std::string method);
	bool client_max_body_size_Check(int body_size);
	std::string autoIdxCheck();
};

#endif
