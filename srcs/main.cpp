#include "Webserv.hpp"

Server server;
int ConfigParser::countLineFile = 0;

void signalHandler(int signum) {
	server.stop();
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
		server.getConfigParser().parse(args.getConfigFilePath());
		if (Logger::getLogDebugState())
			server.getConfigParser().printServers();
		server.init();
		server.run();
	}
	catch (const std::exception &e){
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
