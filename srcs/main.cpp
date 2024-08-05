/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/05 15:30:48 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <arpa/inet.h>  // pour inet_ntop

#define PORT 4321
#define MAX_CONNECTIONS 10000

int main(int ac, char **av)
{
	ArgsManager args(ac, av);

	if (args.isOption("--help"))
		return (args.help(), args.getState());

	// Setup a simple server on port PORT, with a maximum of MAX_CONNECTIONS connections
	SimpleServer server(PORT, MAX_CONNECTIONS);
	
	server.setupServer();
	server.run();

	return (EXIT_SUCCESS);

}
