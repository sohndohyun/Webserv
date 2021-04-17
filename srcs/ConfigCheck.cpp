#include "ConfigCheck.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Utils.hpp"
#include <vector>
#include <iostream>

ConfigCheck::ConfigCheck(ConfigParse &conf, std::string &req_path): conf(conf), req_path(req_path), ori_reqpath(req_path) {}

ConfigCheck::~ConfigCheck() {}

std::string ConfigCheck::findLocation()
{
	if (ori_reqpath == "/")
		return ("/");
	else
	{
		std::map<std::string, ConfigParse::t_location>::iterator iter = conf.loca_map.begin();
		for(; iter != conf.loca_map.end(); iter++)
		{
			if (ori_reqpath.find(iter->first) != std::string::npos)
				return (iter->first);
		}
	}
	return ("");
}

std::string ConfigCheck::getRootURL()
{
	//수정필요
	//conf.server->port[0]
	return ("http://localhost:" + jachoi::to_string(conf.server->port[0]));
}


std::string ConfigCheck::makeAutoIdx(std::string path)
{
	std::string body = "<html>\n\t<head>\n\t\t<title>Index of " + ori_reqpath + "</title>\n\t</head>\n\t<body>\n\t\t<h1>Index of " + ori_reqpath + "</h1>\n\t\t<hr>\n\t\t<pre>";
	std::vector<std::string> dirNames = jachoi::getDirNames(path);
	std::vector<std::string>::iterator iter = dirNames.begin();
	for(; iter != dirNames.end(); iter++)
	{
		if (ori_reqpath == "/" && *iter == "..")
			continue ;
		body += "\n<a href=\"" + getRootURL() + ori_reqpath;
		if (ori_reqpath[ori_reqpath.length() - 1] != '/')
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

std::string ConfigCheck::autoIdxCheck()
{
	std::string location = findLocation();
	std::string body = "";
	std::string path = findPath();
	struct stat sb;

	if (location == "/" || findPath().rfind('/') == conf.server->loca.root.rfind('/'))
	{
		if (conf.server->loca.autoindex)
			body += makeAutoIdx(path);
	}
	else if (location != "")
	{
		if (conf.loca_map[location].autoindex && stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			body += makeAutoIdx(path);
	}
	return (body);
}

void ConfigCheck::isAcceptLanguage(std::string &content_path, std::string req_lang, int is_dir)
{
	if (req_lang == "" || is_dir == 0)
		return ;

	if (ori_reqpath[ori_reqpath.length() - 1] != '/')
		req_path += '/';
	req_path += "index.html";

	std::vector<std::string> langs = jachoi::splitString(req_lang, ',');
	std::vector<std::string>::iterator iter = langs.begin();
	for(; iter != langs.end(); iter++)
	{
		if ((*iter).find("ko") != std::string::npos)
		{
			if (content_path.find("index.html") != std::string::npos)
			{
				content_path = content_path.substr(0, content_path.find("index.html")) + "index_ko.html";
				req_path = req_path.substr(0, req_path.find("index.html")) + "index_ko.html";
			}
			return ;
		}
	}
}



std::string ConfigCheck::makeFilePath(std::string req_lang)
{
	struct stat sb;
	std::string path;
	std::string temp;
	int			is_dir = 0;
	std::string location = findLocation();

	if (location != "" && location != "/")
	{
		if (location.length() < ori_reqpath.length())
			temp = ori_reqpath.substr(location.length() + 1, ori_reqpath.length() - location.length());
		else
			temp = "";
		path = conf.server->loca.root
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
	else
	{
		temp = ori_reqpath.substr(1, ori_reqpath.length() - 1);
		path = conf.server->loca.root + temp;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			is_dir = 1;
			if (path[path.length() - 1] != '/')
				path += '/';
			path += conf.server->loca.index[0];
		}
		if (stat(path.c_str(), &sb) == -1)
			return ("");
	}
	isAcceptLanguage(path, req_lang, is_dir);
	return (path);
}

std::string ConfigCheck::findPath()
{
	std::string path = "";
	std::string temp;
	std::string location = findLocation();

	if (location != "" && location != "/")
	{
		if (location.length() < ori_reqpath.length())
			temp = ori_reqpath.substr(location.length() + 1, ori_reqpath.length() - location.length());
		else
			temp = "";
		path = conf.server->loca.root
				+ conf.loca_map[location].root.substr(1, conf.loca_map[location].root.length() - 1)
				+ temp;
	}
	else
	{
		temp = ori_reqpath.substr(1, ori_reqpath.length() - 1);
		path = conf.server->loca.root + temp;
	}
	return (path);
}

bool ConfigCheck::methodCheck(std::string method, std::vector<std::string> &allow_methods)
{
	std::string location = findLocation();

	if (location == "/" || findPath().rfind('/') == conf.server->loca.root.rfind('/'))
	{
		for(int i = 0; i < (int)conf.server->loca.method.size(); i++)
		{
			if (method == conf.server->loca.method[i])
				return (true);
		}
		allow_methods = conf.server->loca.method;
	}
	else if (location != "")
	{
		for(int i = 0; i < (int)conf.loca_map[location].method.size(); i++)
		{
			if (method == conf.loca_map[location].method[i])
				return (true);
		}
		allow_methods = conf.loca_map[location].method;
	}
	return (false);
}

bool ConfigCheck::client_max_body_size_Check(int body_size)
{
	std::string location = findLocation();

	if (location == "/" || findPath().rfind('/') == conf.server->loca.root.rfind('/'))
	{
		if (conf.server->loca.client_max_body_size > 0 &&
			conf.server->loca.client_max_body_size < body_size)
			return (false);
	}
	else if (location != "")
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

	if (location == "/" || findPath().rfind('/') == conf.server->loca.root.rfind('/'))
	{
		if (ori_reqpath.rfind('.') != std::string::npos &&
			ori_reqpath.substr(ori_reqpath.rfind('.')) == conf.server->loca.cgi)
			return (true);
	}
	else if (location != "")
	{
		if (ori_reqpath.rfind('.') != std::string::npos &&
			ori_reqpath.substr(ori_reqpath.rfind('.')) == conf.loca_map[location].cgi)
			return (true);
	}
	return (false);
}