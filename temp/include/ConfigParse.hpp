#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

# define CONFIG_PATH "./config/config3.ini"

class ConfigParse{
public:
	typedef struct s_location
	{
		std::string	root;
		std::vector<std::string> index;
		std::vector<std::string> method;
		std::string	cgi;
		bool		autoindex;
	}t_location;

	typedef struct s_server
	{
		int			port;
		std::string	host;
		std::string	name;
		std::string	client_max_body_size;
		std::string	error_root;
		std::map<int, std::string> error_page;
		t_location	loca;
	}t_server;

public:
	std::map<std::string, t_location> loca_map;
	t_server *server;

private:
	void sectionParse(std::string str);
	void serverParse(std::vector<std::string> section);
	void locationParse(std::vector<std::string> section);
	std::vector<std::string> splitString(std::string str, char c);

public:
	ConfigParse();
	virtual ~ConfigParse();
};

#endif
