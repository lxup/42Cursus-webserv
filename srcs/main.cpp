/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/26 17:01:52 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <arpa/inet.h>  // pour inet_ntop


int raf()
{
	int port = 4321;
	// int port = 1234;
	int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (!fdSocket)
	{
		Logger::log(Logger::ERROR, "Error with function socket");
		exit(EXIT_FAILURE);
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	if (bind(fdSocket, (const sockaddr *)(&address), sizeof(address)) < 0)
	{
		Logger::log(Logger::ERROR, "Error with function bind");
		exit(EXIT_FAILURE);
	}

	if (listen(fdSocket, 10000) < 0)
	{
		Logger::log(Logger::ERROR, "Error with function listen");
		exit(EXIT_FAILURE);
	}

	bool active = true;
	int connection;
	int maxLine = 4096;
	while (active)
	{
		socklen_t resultLen = sizeof(sockaddr);
		Logger::log(Logger::INFO, "Listening on Port: %d", port);
		if ((connection = accept(fdSocket, (struct sockaddr *)(&address), &resultLen)) < 0)
		{
			Logger::log(Logger::ERROR, "Error with function accept");
			exit(EXIT_FAILURE);
		}

		
		// pour afficher l'adresse ip du client qui se connecte
		
		char client_adress[maxLine + 1];
		inet_ntop(AF_INET, &address, client_adress, maxLine);
		Logger::log(Logger::INFO, "Ip du client: %s", client_adress);


		char buffer[maxLine];
		memset(&buffer, 0, maxLine);
		ssize_t bytesRead;
		while ((bytesRead = read(connection, buffer, maxLine - 1)) > 0){
			Logger::log(Logger::INFO, "Le message fait: %d characteres", bytesRead);
			Logger::log(Logger::INFO, buffer);
			if (buffer[bytesRead - 1] == '\n')
				break;
			memset(&buffer, 0, maxLine);
		}
		 

		// std::string response = "Salut du mini Server de Raf\n";
		const std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \n\n<h1>Hello, World bonjour louis!</h1>";
		if ((send(connection, response.c_str(), response.size(), 0)) < 0){
			Logger::log(Logger::ERROR, "Send function a chier");
			close(connection);
			break;
		}
		close(connection);
	}
	close(fdSocket);

	return (EXIT_SUCCESS);
}

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

	raf();
	// args.summary();

	return (EXIT_SUCCESS);

}
