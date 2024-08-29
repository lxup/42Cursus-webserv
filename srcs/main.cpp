#include "Webserv.hpp"

Server* g_server;

int ConfigParser::countLineFile = 0;

void signalHandler(int signum) {
	g_server->stop();
	Logger::log(Logger::DEBUG, "interrupt signal (%d) received.", signum);
}

int main(int ac, char **av)
{
	Server server;
	g_server = &server;

	ArgsManager args(ac, av);
	
	if (args.isOption("--help"))
		return (args.help(), args.getState());
	
	signal(SIGINT, signalHandler);
	try{
		server.getConfigParser().parse(args.getConfigFilePath());
		Logger::log(Logger::INFO, "Configuration file parsed");
		if (Logger::getLogDebugState())
			server.getConfigParser().printServers();
		server.init();
		server.run();
	} catch (const std::exception &e){
		return (EXIT_FAILURE);
	}
	Logger::log(Logger::DEBUG, "Server stopped");
	
	return (EXIT_SUCCESS);
}
