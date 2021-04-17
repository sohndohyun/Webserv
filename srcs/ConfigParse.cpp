#include "ConfigParse.hpp"
#include <fcntl.h>
#include <unistd.h>
#include "Exception.hpp"
#include "Utils.hpp"

ConfigParse::ConfigParse():
	server(NULL)
{
	int configFD = open(CONFIG_PATH, O_RDONLY);
	char buf;
	if (configFD < 0)
		throw Exception("ConfigParse : Config file doesn't open");

	std::string section_str = "";
	std::string str = "";
	while (read(configFD, &buf, 1))
	{
		str += buf;
		if (str == "\n")
		{
			if (section_str != "")
				sectionParse(section_str);
			section_str = "";
			str = "";
		}
		else if (buf == '\n')
		{
			section_str += str;
			str = "";
		}
	}
	if (str != "")
		section_str += str;
	if (section_str != "")
		sectionParse(section_str);
	close(configFD);
}

void ConfigParse::sectionParse(std::string str)
{
	std::vector<std::string> section = jachoi::splitString(str, '\n');

	if (section[0][0] != '[' || section[0][section[0].size() - 1] != ']')
		throw Exception("ConfigParse: Invalid section name");
	if (section[0] == "[server]")
	{
		if (server)
			throw Exception("ConfigParse: Already have a server section");
		serverParse(section);
	}
	else
	{
		if (loca_map.find(section[0]) != loca_map.end())
			throw Exception("ConfigParse: The same location section cannot exist");
		locationParse(section);
	}
	if (!server)
		throw Exception("ConfigParse: Doesn't exist server section");
}

void ConfigParse::serverParse(std::vector<std::string> section)
{
	server = new t_server;
	int findIdx;
	std::string key;
	std::string value;

	server->loca.client_max_body_size = 0;

	std::vector<std::string>::iterator iter;
	for(iter = section.begin() + 1; iter != section.end(); iter++)
	{
		std::string str = *iter;
		findIdx = str.find("=");
		key = str.substr(0, findIdx);
		value = str.substr(findIdx + 1, str.size() - findIdx);
		if (key == "port")
		{
			std::vector<std::string> ports = jachoi::splitString(value, ' ');
			std::vector<std::string>::iterator ports_iter = ports.begin();
			for(; ports_iter != ports.end(); ports_iter++)
				server->port.push_back(jachoi::stoi(*ports_iter));
		}
		else if (key == "host")
			server->host = value;
		else if (key == "name")
			server->name = value;
		else if (key == "error_root")
			server->error_root = value;
		else if (key == "error_page")
		{
			std::vector<std::string> errors = jachoi::splitString(value, ' ');
			std::vector<std::string>::iterator errors_iter;
			for(errors_iter = errors.begin(); errors_iter != errors.end(); errors_iter++)
			{
				std::string errors_str = *errors_iter;
				findIdx = errors_str.find(":");
				key = errors_str.substr(0, findIdx);
				value = errors_str.substr(findIdx + 1, errors_str.size() - findIdx);
				server->error_page.insert(make_pair(jachoi::stoi(key), value));
			}
		}
		else if (key == "root")
		{
			if (value[value.length() - 1] != '/')
				value += '/';
			server->loca.root = value;
		}
		else if (key == "index")
			server->loca.index = jachoi::splitString(value, ' ');
		else if (key == "method")
		{
			server->loca.method = jachoi::splitString(value, ' ');
			for(std::vector<std::string>::iterator iter_method = server->loca.method.begin(); iter_method != server->loca.method.end(); iter_method++)
			{
				if (isMethod(*iter_method) == false)
					throw Exception("ConfigParse: Invalid method: " + value);
			}
		}
		else if (key == "cgi")
			server->loca.cgi = value;
		else if (key == "autoindex")
		{
			server->loca.autoindex = false;
			if (value == "on")
				server->loca.autoindex = true;
		}
		else if (key == "client_max_body_size")
			server->loca.client_max_body_size = jachoi::stoi(value);
		else
			throw Exception("ConfigParse: Invalid key: " + key);
	}
}

void ConfigParse::locationParse(std::vector<std::string> section)
{
	int findIdx;
	std::string key;
	std::string value;
	t_location loca;
	loca.autoindex = false;

	loca.client_max_body_size = 0;

	std::vector<std::string>::iterator iter;
	for(iter = section.begin() + 1; iter != section.end(); iter++)
	{
		std::string str = *iter;
		findIdx = str.find("=");
		key = str.substr(0, findIdx);
		value = str.substr(findIdx + 1, str.size() - findIdx);
		if (key == "root")
		{
			if (value[value.length() - 1] != '/')
				value += '/';
			loca.root = value;
		}
		else if (key == "index")
			loca.index = jachoi::splitString(value, ' ');
		else if (key == "method")
		{
			loca.method = jachoi::splitString(value, ' ');
			for(std::vector<std::string>::iterator iter_method = loca.method.begin(); iter_method != loca.method.end(); iter_method++)
			{
				if (isMethod(*iter_method) == false)
					throw Exception("ConfigParse: Invalid method: " + value);
			}
		}
		else if (key == "cgi")
			loca.cgi = value;
		else if (key == "autoindex")
		{
			if (value == "on")
				loca.autoindex = true;
		}
		else if (key == "client_max_body_size")
			loca.client_max_body_size = jachoi::stoi(value);
		else
			throw Exception("ConfigParse: Invalid key: " + key);
	}
	loca_map.insert(make_pair(section[0].substr(1, section[0].length() - 2), loca));
}

bool ConfigParse::isMethod(std::string method)
{
	const std::string methods[] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"
	};
	for(int i = 0; i < 8; i++)
	{
		if (methods[i] == method)
			return (true);
	}
	return (false);
}

ConfigParse::~ConfigParse()
{
	if (server)
		delete server;
}