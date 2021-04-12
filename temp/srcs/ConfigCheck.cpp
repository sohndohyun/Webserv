#include "ConfigCheck.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "RequestParser.hpp"
#include "Utils.hpp"
#include <vector>
#include <iostream>

ConfigCheck::ConfigCheck(ConfigParse &conf, std::string req_path): conf(conf), req_path(req_path) {}

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

std::string ConfigCheck::getRootURL()
{
	return ("http://localhost:" + std::to_string(conf.server->port));
}


std::string ConfigCheck::makeAutoIdx(std::string path)
{
	std::string body = "<html>\n\t<head>\n\t\t<title>Index of " + req_path + "</title>\n\t</head>\n\t<body>\n\t\t<h1>Index of " + req_path + "</h1>\n\t\t<hr>\n\t\t<pre>";
	std::vector<std::string> dirNames = jachoi::getDirNames(path);
	std::vector<std::string>::iterator iter = dirNames.begin();
	for(; iter != dirNames.end(); iter++)
	{
		if (req_path == "/" && *iter == "..")
			continue ;
		body += "\n<a href=\"" + getRootURL() + req_path;
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

std::string ConfigCheck::makeFilePath()
{
	struct stat sb;
	std::string path;
	std::string temp;
	std::string location = findLocation();

	if (location != "" && location != "/")
	{
		if (location.length() < req_path.length())
			temp = req_path.substr(location.length() + 1, req_path.length() - location.length());
		else
			temp = "";
		path = conf.server->loca.root
				+ conf.loca_map[location].root.substr(1, conf.loca_map[location].root.length() - 1)
				+ temp;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			if (path[path.length() - 1] != '/')
				path += '/';
			path += conf.loca_map[location].index[0];
		}
		if (stat(path.c_str(), &sb) == -1)
			return ("");
	}
	else
	{
		temp = req_path.substr(1, req_path.length() - 1);
		path = conf.server->loca.root + temp;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			if (path[path.length() - 1] != '/')
				path += '/';
			path += conf.server->loca.index[0];
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
		else
			temp = "";
		path = conf.server->loca.root
				+ conf.loca_map[location].root.substr(1, conf.loca_map[location].root.length() - 1)
				+ temp;
	}
	else
	{
		temp = req_path.substr(1, req_path.length() - 1);
		path = conf.server->loca.root + temp;
	}
	return (path);
}

bool ConfigCheck::methodCheck(std::string method)
{
	std::string location = findLocation();
	if (location == "/" || findPath().rfind('/') == conf.server->loca.root.rfind('/'))
	{
		for(int i = 0; i < (int)conf.server->loca.method.size(); i++)
		{
			if (method == conf.server->loca.method[i])
				return (true);
		}
	}
	else if (location != "")
	{
		for(int i = 0; i < (int)conf.loca_map[location].method.size(); i++)
		{
			if (method == conf.loca_map[location].method[i])
				return (true);
		}
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