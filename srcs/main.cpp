/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/07 12:25:55 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int main(int ac, char **av)
{
	ArgsManager args(ac, av);

	if (args.isOption("--help"))
		return (args.help(), args.getState());

	try
	{
		ConfigParser configParser(args.getConfigFilePath());
		// throw WebservException(Logger::FATAL, "This is an error");
	}
	catch (const WebservException &e)
	{
		Logger::log(e.getLogLevel(), e.what());
		return (EXIT_FAILURE);
	}

	// args.summary();

	return (EXIT_SUCCESS);
}
