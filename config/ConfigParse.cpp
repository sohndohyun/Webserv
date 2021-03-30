#include "ConfigParse.hpp"

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
	if (section_str != "")
		sectionParse(section_str);
	close(configFD);
}

void ConfigParse::sectionParse(std::string str)
{
	std::vector<std::string> section = splitString(str, '\n');

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

	std::vector<std::string>::iterator iter;
	for(iter = section.begin() + 1; iter != section.end(); iter++)
	{
		std::string str = *iter;
		findIdx = str.find("=");
		key = str.substr(0, findIdx);
		value = str.substr(findIdx + 1, str.size() - findIdx);
		if (key == "port")
			server->port = std::stoi(value);
		else if (key == "host")
			server->host = value;
		else if (key == "name")
			server->name = value;
		else if (key == "client_max_body_size")
			server->client_max_body_size = value;
		else if (key == "error_root")
			server->error_root = value;
		else if (key == "error_page")
		{
			std::vector<std::string> errors = splitString(value, ' ');
			std::vector<std::string>::iterator errors_iter;
			for(errors_iter = errors.begin(); errors_iter != errors.end(); errors_iter++)
			{
				std::string errors_str = *errors_iter;
				findIdx = errors_str.find(":");
				key = errors_str.substr(0, findIdx);
				value = errors_str.substr(findIdx + 1, errors_str.size() - findIdx);
				server->error_page.insert(make_pair(std::stoi(key), value));
			}
		}
		else if (key == "root")
			server->loca.root = value;
		else if (key == "index")
			server->loca.index = splitString(value, ' ');
		else if (key == "method")
			server->loca.method = splitString(value, ' ');
		else if (key == "cgi")
			server->loca.cgi = value;
		else if (key == "autoindex")
		{
			server->loca.autoindex = false;
			if (value == "on")
				server->loca.autoindex = true;
		}
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

	std::vector<std::string>::iterator iter;
	for(iter = section.begin() + 1; iter != section.end(); iter++)
	{
		std::string str = *iter;
		findIdx = str.find("=");
		key = str.substr(0, findIdx);
		value = str.substr(findIdx + 1, str.size() - findIdx);
		if (key == "root")
			loca.root = value;
		else if (key == "index")
			loca.index = splitString(value, ' ');
		else if (key == "method")
			loca.method = splitString(value, ' ');
		else if (key == "cgi")
			loca.cgi = value;
		else if (key == "autoindex")
		{
			if (value == "on")
				loca.autoindex = true;
		}
		else
			throw Exception("ConfigParse: Invalid key: " + key);
	}
	loca_map.insert(make_pair(section[0], loca));
}

ConfigParse::~ConfigParse()
{
	if (server)
		delete server;
}

std::vector<std::string> ConfigParse::splitString(std::string str, char c)
{
	std::vector<std::string> rtn;
	int start, end;
	for(start = 0; str[start] == c && start < (int)str.length(); start++) ;
	for(end = str.length() - 1; str[end] == c && end >= 0; end--) ;
	str = str.substr(start, end - start + 1);

	std::string tmp;
	tmp += str[0];
	for(int i = 1; i < (int)str.length(); i++)
	{
		if (str[i] == c && str[i - 1] == c)
			continue ;
		tmp += str[i];
	}
	str = tmp;

	int count = 0;
	for(int idx = 0; str[idx]; idx++)
	{
		if (str[idx] == c)
			count++;
	}
	if (count != 0 || (count == 0 && str != ""))
		count++;

	for(int i = 0; i < count; i++)
	{
		int len = str.find(c);
		rtn.push_back(str.substr(0, len));
		str = str.substr(len + 1, str.size() - len);
	}
	return (rtn);
}
