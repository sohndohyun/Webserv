#include "WebServer_bonus.hpp"
#include "ConfigParse_bonus.hpp"
#include <iostream>
#include "Utils_bonus.hpp"
#include "Plugin.hpp"

int main(int argc, char *argv[])
{
	(void)argc;
	try
	{
		Plugin plugin(argv);
		plugin.parsing();

		ConfigParse cp(plugin.plugin.config);
		WebServer webServer(cp, plugin.plugin);
		webServer.run("0.0.0.0", cp.port);
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
