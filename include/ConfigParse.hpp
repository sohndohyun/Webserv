#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

# define CONFIG_PATH "./config/config.ini"

class ConfigParse{
public:
	typedef struct s_location
	{
		std::string	root;
		std::vector<std::string> index;
		std::vector<std::string> method;
		std::string	cgi;
		bool		autoindex;
		int	client_max_body_size;
	}t_location;

	typedef struct s_server
	{
		std::vector<int> port;
		std::string	host;
		std::string	name;
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
	bool isMethod(std::string method);

public:
	ConfigParse();
	virtual ~ConfigParse();
};

#endif
