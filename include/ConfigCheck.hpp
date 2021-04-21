#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <string>
#include "ConfigParse.hpp"
#include "AServer.hpp"

class ConfigCheck
{
private:
	ConfigParse::t_conf &conf;
	std::string req_path;

private:
	std::string findLocation();
	std::string getRootURL();
	std::string makeAutoIdx(std::string path);
	bool auth_ID_PWD_check(std::string auth_path, std::string auth_str);

public:
	ConfigCheck(ConfigParse::t_conf &conf, std::string req_path);
	~ConfigCheck();

	std::string makeFilePath(int &is_dir);
	std::string findPath();

	bool methodCheck(std::string method, std::vector<std::string> &allow_methods);
	bool client_max_body_size_Check(int body_size);
	bool cgiCheck();
	bool AuthorizationCheck(std::string auth_str);
	bool analysisCheck();
	std::string makeAnalysisHTML(AServer::t_analysis analysis);
	std::string autoIdxCheck();
};

#endif
