#include "Webserv.hpp"

Server* serverInstance = NULL;
int ConfigParser::countLineFile = 0;

void signalHandler(int signum) {
	serverInstance->stop();
	Logger::log(Logger::DEBUG, "interrupt signal (%d) received.", signum);
}

int main(int ac, char **av)
{
	ArgsManager args(ac, av);
	
	if (args.isOption("--help"))
		return (args.help(), args.getState());

	ConfigParser configs(args.getConfigFilePath());
	Server server;
	serverInstance = &server;
	
	signal(SIGINT, signalHandler);
	
	try{
		// 1- Parsing
		configs.parse();
		configs.printServers();

		// 2- Initialisation et lancement du Server
		server.init(configs.getConfigs());
		server.run();
	}
	catch (const std::exception &e){
		return (EXIT_FAILURE);
	}
	// catch (const WebservException &e){
	// 	Logger::log(e.getLogLevel(), e.what());
	// 	return (EXIT_FAILURE);
	// }



	// args.summary();

	return (EXIT_SUCCESS);
}
