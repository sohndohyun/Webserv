#include "ConfigCheck.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ConfigParse.hpp"
#include "RequestParser.hpp"

ConfigCheck::ConfigCheck(ConfigParse &conf): conf(conf) {}

ConfigCheck::~ConfigCheck() {}

std::string ConfigCheck::makeFilePath(std::string req_path)
{
	struct stat sb;
	std::string path;

	std::map<std::string, ConfigParse::t_location>::iterator iter = conf.loca_map.begin();
	for(; iter != conf.loca_map.end(); iter++)
	{
		if (req_path.find(iter->first) != std::string::npos)
		{
			if (iter->first.length() < req_path.length())
				req_path = req_path.substr((iter->first).length() + 1, req_path.length() - (iter->first).length());
			else
				req_path = "";
			path = conf.server->loca.root
					+ iter->second.root.substr(1, iter->second.root.length() - 1)
					+ req_path;
			if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			{
				if (path[path.length() - 1] != '/')
					path += '/';
				path += iter->second.index[0];
			}
			if (stat(path.c_str(), &sb) == -1)
				return ("");
			return (path);
		}
	}
	req_path = req_path.substr(1, req_path.length() - 1);
	path = conf.server->loca.root + req_path;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		if (path[path.length() - 1] != '/')
			path += '/';
		path += conf.server->loca.index[0];
	}
	if (stat(path.c_str(), &sb) == -1)
		return ("");
	return (path);
}

std::string ConfigCheck::findPath(std::string req_path)
{
	std::string path;

	std::map<std::string, ConfigParse::t_location>::iterator iter = conf.loca_map.begin();
	for(; iter != conf.loca_map.end(); iter++)
	{
		if (req_path.find(iter->first) != std::string::npos)
		{
			if (iter->first.length() < req_path.length())
				req_path = req_path.substr((iter->first).length() + 1, req_path.length() - (iter->first).length());
			else
				req_path = "";
			path = conf.server->loca.root
					+ iter->second.root.substr(1, iter->second.root.length() - 1)
					+ req_path;
			return (path);
		}
	}
	req_path = req_path.substr(1, req_path.length() - 1);
	path = conf.server->loca.root + req_path;
	return (path);
}

bool ConfigCheck::methodCheck(std::string method, std::string req_path)
{
	if (req_path == "/")
	{
		for(int i = 0; i < (int)conf.server->loca.method.size(); i++)
		{
			if (method == conf.server->loca.method[i])
				return (true);
		}
		return (false);
	}
	std::map<std::string, ConfigParse::t_location>::iterator iter = conf.loca_map.begin();
	for(; iter != conf.loca_map.end(); iter++)
	{
		if (req_path.find(iter->first) != std::string::npos)
			return (true);
	}
	return (false);
}
