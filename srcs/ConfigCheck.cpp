#include "ConfigCheck.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Utils.hpp"
#include "FileIO.hpp"
#include <vector>
#include <iostream>

ConfigCheck::ConfigCheck(ConfigParse::t_conf &conf, std::string req_path): conf(conf), req_path(req_path) {}

ConfigCheck::~ConfigCheck() {}

std::string ConfigCheck::findLocation()
{
	if (req_path == "/")
		return ("/");
	else
	{
		std::map<std::string, ConfigParse::t_location>::iterator iter = conf.loca_map.begin();
		for(; iter != conf.loca_map.end(); iter++)
		{
			if (req_path.find(iter->first) != std::string::npos)
				return (iter->first);
		}
	}
	return ("");
}

std::string ConfigCheck::getRootURL(int port)
{
	return ("http://localhost:" + jachoi::to_string(port));
}


std::string ConfigCheck::makeAutoIdx(std::string path, int port)
{
	std::string body = "<html>\n\t<head>\n\t\t<title>Index of " + req_path + "</title>\n\t</head>\n\t<body>\n\t\t<h1>Index of " + req_path + "</h1>\n\t\t<hr>\n\t\t<pre>";
	std::vector<std::string> dirNames = jachoi::getDirNames(path);
	std::vector<std::string>::iterator iter = dirNames.begin();
	for(; iter != dirNames.end(); iter++)
	{
		if (req_path == "/" && *iter == "..")
			continue ;
		body += "\n<a href=\"" + getRootURL(port) + req_path;
		if (req_path[req_path.length() - 1] != '/')
			body += "/";
		std::map<std::string, ConfigParse::t_location>::iterator iter_loca = conf.loca_map.begin();
		int tmp = 0;
		for(; iter_loca != conf.loca_map.end(); iter_loca++)
		{
			if (iter_loca->second.root == "/" + *iter + "/")
			{
				body += iter_loca->first.substr(1, iter_loca->first.length() - 1) + "\">" + iter_loca->first.substr(1, iter_loca->first.length() - 1) + "</a>";
				tmp = 1;
				break ;
			}
		}
		if (tmp == 0)
			body += *iter + "\">" + *iter + "</a>";
	}
	body += "</pre>\n\t\t<hr>\n\t</body>\n<html>";
	return (body);
}

std::string ConfigCheck::autoIdxCheck(int port)
{
	std::string location = findLocation();
	std::string body = "";
	std::string path = findPath();
	struct stat sb;

	if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		if (conf.server.loca.autoindex)
			body += makeAutoIdx(path, port);
	}
	else if (location != "")
	{
		if (conf.loca_map[location].autoindex && stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			body += makeAutoIdx(path, port);
	}
	return (body);
}



std::string ConfigCheck::makeFilePath(int &is_dir)
{
	struct stat sb;
	std::string path = "";
	std::string temp;
	std::string location = findLocation();

	if (location != "" && location != "/")
	{
		if (location.length() < req_path.length())
			temp = req_path.substr(location.length() + 1, req_path.length() - location.length());
		else
			temp = "";
		path = conf.server.loca.root
				+ conf.loca_map[location].root.substr(1, conf.loca_map[location].root.length() - 1)
				+ temp;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			is_dir = 1;
			if (path[path.length() - 1] != '/')
				path += '/';
			path += conf.loca_map[location].index[0];
		}
		if (stat(path.c_str(), &sb) == -1)
			return ("");
	}
	else if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		temp = req_path.substr(1, req_path.length() - 1);
		path = conf.server.loca.root + temp;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			is_dir = 1;
			if (path[path.length() - 1] != '/')
				path += '/';
			path += conf.server.loca.index[0];
		}
		if (stat(path.c_str(), &sb) == -1)
			return ("");
	}
	return (path);
}

std::string ConfigCheck::findPath()
{
	std::string path = "";
	std::string temp;
	std::string location = findLocation();

	if (location != "" && location != "/")
	{
		if (location.length() < req_path.length())
			temp = req_path.substr(location.length() + 1, req_path.length() - location.length());
		else if (conf.loca_map[location].root == "./")
			temp = location.substr(1, location.length() - 1);
		else
			temp = "";
		int temp_len = conf.loca_map[location].root.find('/');
		path = conf.server.loca.root + conf.loca_map[location].root.substr(temp_len + 1, conf.loca_map[location].root.length() - (temp_len + 1)) + temp;
	}
	else
	{
		temp = req_path.substr(1, req_path.length() - 1);
		path = conf.server.loca.root + temp;
	}
	return (path);
}

bool ConfigCheck::methodCheck(std::string method, std::vector<std::string> &allow_methods)
{
	std::string location = findLocation();

	if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		for(int i = 0; i < (int)conf.server.loca.method.size(); i++)
		{
			if (method == conf.server.loca.method[i])
				return (true);
		}
		allow_methods = conf.server.loca.method;
	}
	if (location != "" && location != "/")
	{
		for(int i = 0; i < (int)conf.loca_map[location].method.size(); i++)
		{
			if (method == conf.loca_map[location].method[i])
				return (true);
		}
		allow_methods.clear();
		allow_methods = conf.loca_map[location].method;
	}
	return (false);
}

bool ConfigCheck::client_max_body_size_Check(int body_size)
{
	std::string location = findLocation();

	if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		if (conf.server.loca.client_max_body_size > 0 &&
			conf.server.loca.client_max_body_size < body_size)
			return (false);
	}
	if (location != "" && location != "/")
	{
		if (conf.loca_map[location].client_max_body_size > 0 &&
			conf.loca_map[location].client_max_body_size < body_size)
			return (false);
	}
	return (true);
}

bool ConfigCheck::cgiCheck()
{
	std::string location = findLocation();

	if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		if (req_path.rfind('.') != std::string::npos &&
			req_path.substr(req_path.rfind('.')) == conf.server.loca.cgi)
			return (true);
	}
	if (location != "" && location != "/")
	{
		if (req_path.rfind('.') != std::string::npos &&
			req_path.substr(req_path.rfind('.')) == conf.loca_map[location].cgi)
			return (true);
	}
	return (false);
}

bool ConfigCheck::auth_ID_PWD_check(std::string htpasswd, std::string auth_str)
{
	if (auth_str.find("Basic") == std::string::npos)
		return false;

	std::string decodeStr = "";
	auth_str = auth_str.substr(auth_str.find(' ') + 1, auth_str.length() - (auth_str.find(' ') + 1));

	jachoi::base64Decode(auth_str, (int)auth_str.length(), decodeStr);

	std::vector<std::string> lists = jachoi::splitString(htpasswd, '\n');
	for(int i = 0; i < (int)lists.size(); i++)
	{
		std::vector<std::string> id_pwd = jachoi::splitString(lists[i], ':');
		std::string idpwStr;
		jachoi::base64Decode(id_pwd[1], (int)id_pwd[1].length(), idpwStr);
		idpwStr = id_pwd[0] + ":" + idpwStr;
		if (decodeStr == idpwStr)
			return true;
	}
	return false;
}

bool ConfigCheck::AuthorizationCheck(std::string auth_str)
{
	std::string location = findLocation();
	if (location == "/" || findPath().rfind('/') == conf.server.loca.root.rfind('/'))
	{
		if (conf.server.loca.auth_basic_user_file != "")
			return (auth_ID_PWD_check(conf.htpasswd["server"], auth_str));
		else
			return true;
	}
	else if (location != "")
	{
		if (conf.loca_map[location].auth_basic_user_file != "")
			return (auth_ID_PWD_check(conf.htpasswd[location], auth_str));
		else
			return true;
	}
	return false;
}

std::string ConfigCheck::makeAnalysisHTML(AServer::t_analysis analysis)
{
	std::string str = "<html>\n\t<head>\n\t\t<title>Analysis</title>\n\t</head>\n\t<body>\n\t\t<h1>Analysis</h1>";

	if (analysis.referer.size() != 0)
		str += "\n\t\t<hr>\n\t\t<h2>where do these people come from?</h2>\n\t\t<pre><b>";
	int total = 0;
	double result = 0;
	std::map<std::string, int>::iterator ref_iter = analysis.referer.begin();
	for(; ref_iter != analysis.referer.end(); ref_iter++)
		total += ref_iter->second;
	for(ref_iter = analysis.referer.begin(); ref_iter != analysis.referer.end(); ref_iter++)
	{
		result = (double)ref_iter->second / (double)total * 100.0;
		str += ref_iter->first + "\t: " + jachoi::to_string((int)result) + "%\n";
	}
	if (analysis.referer.size() != 0)
		str += "</b></pre>";

	str += "\n\t\t<hr>\n\t\t<h2>Which browser do people use the most?</h2>\n\t\t<pre><b>";

	std::map<std::string, int>::iterator user_iter = analysis.user_agent.begin();
	for(total = 0; user_iter != analysis.user_agent.end(); user_iter++)
		total += user_iter->second;
	for(user_iter = analysis.user_agent.begin(); user_iter != analysis.user_agent.end(); user_iter++)
	{
		result = (double)user_iter->second / (double)total * 100.0;
		str += user_iter->first + "\t: " + jachoi::to_string((int)result) + "%\n";
	}
	str += "</b></pre>\n\t</body>\n</html>";

	return str;
}

bool ConfigCheck::analysisCheck()
{
	std::string location = findLocation();
	if (location == "/analysis")
		return true;
	return false;
}