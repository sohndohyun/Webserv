#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <string>
class ConfigParse;
class RequestParser;

class ConfigCheck
{
private:
	ConfigParse &conf;

public:
	ConfigCheck(ConfigParse &conf);
	~ConfigCheck();

	std::string makeFilePath(std::string req_path);
	std::string findPath(std::string req_path);
	bool methodCheck(std::string method, std::string req_path);
};

#endif
