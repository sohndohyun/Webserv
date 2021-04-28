#ifndef PLUGIN_HPP
# define PLUGIN_HPP

# include <string>
# include <vector>

class Plugin
{
public:
	std::vector<std::string> argv;

	typedef struct s_plugin
	{
		bool auth;
		bool analysis;
		bool index_ko;
		int thread;
		std::string config;
	}t_plugin;
	t_plugin plugin;

public:
	Plugin(char **argv);
	~Plugin();

	void parsing();
};

#endif