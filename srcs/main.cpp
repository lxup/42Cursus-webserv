#include "Webserv.hpp"

Server g_server;
int ConfigParser::countLineFile = 0;

void signalHandler(int signum) {
	g_server.stop();
	Logger::log(Logger::DEBUG, "interrupt signal (%d) received.", signum);
}

int main(int ac, char **av)
{

	ArgsManager args(ac, av);
	
	if (args.isOption("--help"))
		return (args.help(), args.getState());
	
	signal(SIGINT, signalHandler);
	try{
		// 1- Parsing
		g_server.getConfigParser().parse(args.getConfigFilePath());
		if (Logger::getLogDebugState())
			g_server.getConfigParser().printServers();
		g_server.init();
		g_server.run();
	} catch (const std::exception &e){
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
