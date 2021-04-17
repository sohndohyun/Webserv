#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <string>
#include "ConfigParse.hpp"
class RequestParser;

class ConfigCheck
{
private:
	ConfigParse::t_conf &conf;
	std::string &req_path;
	std::string ori_reqpath;

private:
	std::string findLocation();
	std::string getRootURL();
	std::string makeAutoIdx(std::string path);
	void isAcceptLanguage(std::string &content_path, std::string req_lang, int is_dir);

public:
	ConfigCheck(ConfigParse::t_conf &conf, std::string &req_path);
	~ConfigCheck();

	std::string makeFilePath(std::string req_lang);
	std::string findPath();

	bool methodCheck(std::string method, std::vector<std::string> &allow_methods);
	bool client_max_body_size_Check(int body_size);
	std::string autoIdxCheck();
	bool cgiCheck();
};

#endif
