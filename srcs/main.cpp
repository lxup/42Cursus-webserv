/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/13 15:58:28 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Server* serverInstance = NULL;

void signalHandler(int signum) {
	Logger::log(Logger::INFO, "interrupt signal (%d) received.", signum);
   serverInstance->stop();
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
		server.init(configs.getServersConfig());
		server.run();
	}
	catch (const WebservException &e){
		Logger::log(e.getLogLevel(), e.what());
		return (EXIT_FAILURE);
	}



	// args.summary();

	return (EXIT_SUCCESS);
}
