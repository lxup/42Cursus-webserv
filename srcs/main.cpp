/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/12 17:07:50 by rgiraud          ###   ########.fr       */
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
