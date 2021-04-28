#include "Plugin.hpp"
#include "Utils_bonus.hpp"
#include "Exception_bonus.hpp"

Plugin::Plugin(char **argv)
{
	plugin.auth = true;
	plugin.analysis = true;
	plugin.index_ko = true;
	plugin.thread = 0;
	plugin.config = "";

	for(int i = 1; argv[i]; i++)
		this->argv.push_back(argv[i]);
}

Plugin::~Plugin() {}



void Plugin::parsing()
{
	for(int i = 0; i < (int)argv.size(); i++)
	{
		if (argv[i] == "-authorization" || argv[i] == "-auth")
		{
			if (argv[++i] == "off")
				plugin.auth = false;
		}
		else if (argv[i] == "-analysis" || argv[i] == "-a")
		{
			if (argv[++i] == "off")
				plugin.analysis = false;
		}
		else if (argv[i] == "-index_ko" || argv[i] == "-i")
		{
			if (argv[++i] == "off")
				plugin.index_ko = false;
		}
		else if (argv[i] == "-thread" || argv[i] == "-t")
			plugin.thread = utils::stoi(argv[++i]);
		else if (argv[i] == "-config" || argv[i] == "-c")
			plugin.config = argv[++i];
		else if (argv[i] == "-help" || argv[i] == "-h")
			throw Exception("-auth or -authorization : authorization option on/off (default=on)\n-a or -analysis : analysis option on/off (default=on)\n-i or -index_ko : index_ko option on/off (default=on)\n-t or -thread : number of thread\n-c or -config : config file path");
		else
			throw Exception("Let's try -h or -help");
	}
}