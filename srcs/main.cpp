#include "Webserv.hpp"

int main(int ac, char **av)
{
	ArgsManager args(ac, av);

	if (args.isOption("--help"))
		return (args.help(), args.getState());

	try
	{
		ConfigParser configParser(args.getConfigFilePath());
		configParser.printServers();
	}
	catch (const WebservException &e)
	{
		Logger::log(e.getLogLevel(), e.what());
		return (EXIT_FAILURE);
	}

	// args.summary();

	return (EXIT_SUCCESS);
}
