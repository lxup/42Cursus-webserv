/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/13 12:15:27 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

#define PORT 4321
#define MAX_CONNECTIONS 10000

int main(int ac, char **av)
{
	ArgsManager args(ac, av);

	if (args.isOption("--help"))
		return (args.help(), args.getState());

	ConfigParser configs(args.getConfigFilePath());
	Server server;

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
