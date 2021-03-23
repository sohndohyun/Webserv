/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinkim <jinkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 02:07:57 by jinkim            #+#    #+#             */
/*   Updated: 2021/03/17 01:30:01 by jinkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParse.hpp"

ConfigParse::ConfigParse():
	server(NULL)
{
	int configFD = open(CONFIG_PATH, O_RDONLY);
	char buf;
	if (configFD < 0)
		throw ConfigParse::FileNotOpenException();

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
	std::string *section = splitString(str, '\n');

	if (section[0][0] != '[' || section[0][section[0].size() - 1] != ']')
		throw ConfigParse::InvalidConfigException();

	if (section[0] == "[server]")
	{
		if (server)
			throw ConfigParse::InvalidConfigException();
		serverParse(section);
	}
	else
	{
		if (loca_map.find(section[0]) != loca_map.end())
			throw ConfigParse::InvalidConfigException();
		locationParse(section);
	}
	delete[] section;
}

void ConfigParse::serverParse(std::string *section)
{
	server = new t_server;
	int findIdx;
	std::string key;
	std::string value;

	for(int idx = 1; section[idx] != ""; idx++)
	{
		findIdx = section[idx].find("=");
		key = section[idx].substr(0, findIdx);
		value = section[idx].substr(findIdx + 1, section[idx].size() - findIdx);
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
			std::string *errors = splitString(value, ' ');
			for(int i = 0; errors[i] != ""; i++)
			{
				findIdx = errors[i].find(":");
				key = errors[i].substr(0, findIdx);
				value = errors[i].substr(findIdx + 1, errors[i].size() - findIdx);
				server->error_page.insert(make_pair(std::stoi(key), value));
			}
			delete[] errors;
		}
		else if (key == "root")
			server->loca.root = value;
		else if (key == "index")
		{
			std::string *indexes = splitString(value, ' ');
			for(int i = 0; indexes[i] != ""; i++)
				server->loca.index.push_back(indexes[i]);
			delete[] indexes;
		}
		else if (key == "method")
		{
			std::string *methods = splitString(value, ' ');
			for(int i = 0; methods[i] != ""; i++)
				server->loca.method.push_back(methods[i]);
			delete[] methods;
		}
		else if (key == "cgi")
			server->loca.cgi = value;
		else if (key == "autoindex")
		{
			server->loca.autoindex = false;
			if (value == "on")
				server->loca.autoindex = true;
		}
		else
			throw ConfigParse::InvalidConfigException();
	}
}

void ConfigParse::locationParse(std::string *section)
{
	int findIdx;
	std::string key;
	std::string value;
	t_location loca;
	loca.autoindex = false;

	for(int idx = 1; section[idx] != ""; idx++)
	{
		findIdx = section[idx].find("=");
		key = section[idx].substr(0, findIdx);
		value = section[idx].substr(findIdx + 1, section[idx].size() - findIdx);
		if (key == "root")
			loca.root = value;
		else if (key == "index")
		{
			std::string *indexes = splitString(value, ' ');
			for(int i = 0; indexes[i] != ""; i++)
				loca.index.push_back(indexes[i]);
			delete[] indexes;
		}
		else if (key == "method")
		{
			std::string *methods = splitString(value, ' ');
			for(int i = 0; methods[i] != ""; i++)
				loca.method.push_back(methods[i]);
			delete[] methods;
		}
		else if (key == "cgi")
			loca.cgi = value;
		else if (key == "autoindex")
		{
			if (value == "on")
				loca.autoindex = true;
		}
		else
			throw ConfigParse::InvalidConfigException();
	}
	loca_map.insert(make_pair(section[0], loca));
}

ConfigParse::~ConfigParse()
{
	if (server)
		delete server;
}

const char *ConfigParse::FileNotOpenException::what() const throw()
{
	return ("Exception : File doesn't open");
}

const char *ConfigParse::InvalidConfigException::what() const throw()
{
	return ("Exception : Invalid Config File");
}

std::string *ConfigParse::splitString(std::string str, char c)
{
	int count = 0;
	int idx = 0;
	for(; str[idx]; idx++)
	{
		if (str[idx] == c)
			count++;
	}
	if (count != 0 || (count == 0 && str != ""))
		count++;

	std::string *rtn = new std::string[count + 1];
	int len = 0;
	idx = 0;
	for(int i = 0; i < count; i++)
	{
		len = str.find(c);
		if (str[0] != c)
			rtn[idx++] = str.substr(0, len);
		str = str.substr(len + 1, str.size() - len);
	}
	rtn[idx] = "";
	return (rtn);
}
