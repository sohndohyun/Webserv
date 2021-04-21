#include "ConfigParse.hpp"
#include <fcntl.h>
#include <unistd.h>
#include "Exception.hpp"
#include "Utils.hpp"
#include "FileIO.hpp"

ConfigParse::ConfigParse(): _confIdx(-1)
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

	
	
	for(size_t i = 0; i < conf.size(); i++)
	{
		if (conf[i].server.loca.auth_basic_user_file != "")
		{
			jachoi::FileIO(conf[i].server.loca.auth_basic_user_file).read(conf[i].htpasswd["server"]);
		}
		std::map<std::string, t_location>::iterator auth_iter = conf[i].loca_map.begin();
		for(; auth_iter != conf[i].loca_map.end(); auth_iter++)
		{
			if (auth_iter->second.auth_basic_user_file != "")
				jachoi::FileIO(auth_iter->second.auth_basic_user_file).read(conf[i].htpasswd[auth_iter->first]);
		}
	}
}

void ConfigParse::sectionParse(std::string str)
{
	std::vector<std::string> section = jachoi::splitString(str, '\n');

	if (section[0][0] != '[' || section[0][section[0].size() - 1] != ']')
		throw Exception("ConfigParse: Invalid section name");

	if (section[0] == "[server]")
	{
		t_conf new_server;
		conf.push_back(new_server);
		_confIdx++;
		serverParse(section);
	}
	else
	{
		if (_confIdx == -1)
			throw Exception("ConfigParse: Invalid config form: [server] must come first");
		if (conf[_confIdx].loca_map.find(section[0]) != conf[_confIdx].loca_map.end())
			throw Exception("ConfigParse: The same location section cannot exist");
		locationParse(section);
	}
}

void ConfigParse::serverParse(std::vector<std::string> section)
{
	int findIdx;
	std::string key;
	std::string value;

	conf[_confIdx].server.loca.client_max_body_size = 0;
	conf[_confIdx].server.loca.auth_basic_user_file = "";

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
			{
				int _port = jachoi::stoi(*ports_iter);
				conf[_confIdx].server.port.push_back(_port);
				port.push_back(_port);
			}
		}
		else if (key == "host")
			conf[_confIdx].server.host = value;
		else if (key == "name")
			conf[_confIdx].server.name = value;
		else if (key == "error_root")
			conf[_confIdx].server.error_root = value;
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
				conf[_confIdx].server.error_page.insert(make_pair(jachoi::stoi(key), value));
			}
		}
		else if (key == "root")
		{
			if (value[value.length() - 1] != '/')
				value += '/';
			conf[_confIdx].server.loca.root = value;
		}
		else if (key == "index")
			conf[_confIdx].server.loca.index = jachoi::splitString(value, ' ');
		else if (key == "method")
		{
			conf[_confIdx].server.loca.method = jachoi::splitString(value, ' ');
			for(std::vector<std::string>::iterator iter_method = conf[_confIdx].server.loca.method.begin(); iter_method != conf[_confIdx].server.loca.method.end(); iter_method++)
			{
				if (isMethod(*iter_method) == false)
					throw Exception("ConfigParse: Invalid method: " + value);
			}
		}
		else if (key == "cgi")
			conf[_confIdx].server.loca.cgi = value;
		else if (key == "autoindex")
		{
			conf[_confIdx].server.loca.autoindex = false;
			if (value == "on")
				conf[_confIdx].server.loca.autoindex = true;
		}
		else if (key == "client_max_body_size")
			conf[_confIdx].server.loca.client_max_body_size = jachoi::stoi(value);
		else if (key == "auth_basic")
			conf[_confIdx].server.loca.auth_basic = value;
		else if (key == "auth_basic_user_file")
			conf[_confIdx].server.loca.auth_basic_user_file = value;
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
	loca.auth_basic_user_file = "";

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
		else if (key == "auth_basic")
			loca.auth_basic = value;
		else if (key == "auth_basic_user_file")
			loca.auth_basic_user_file = value;
		else
			throw Exception("ConfigParse: Invalid key: " + key);
	}
	conf[_confIdx].loca_map.insert(make_pair(section[0].substr(1, section[0].length() - 2), loca));
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
}