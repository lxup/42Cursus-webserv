/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 14:09:48 by rgiraud           #+#    #+#             */
/*   Updated: 2024/08/02 16:53:21 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLESERVER_HPP
# define SIMPLESERVER_HPP

#include "Logger.hpp"
#include "Defines.hpp"
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <arpa/inet.h>  // pour inet_ntop
#include <string>

#define BUFFER_SIZE 4096


class SimpleServer
{
	
private:
	const int _port;
	const int _maxConnection;
	sockaddr_in _server_address;
	int _server_socket;
	int _client_socket;
	int _addr_size;
	
	int check(int ret, std::string msg);
	int acceptConnection(void);
	void handleConnection();
	void showIpClient(void);
	
public:
	SimpleServer(int port, int maxConnection);
	~SimpleServer();

	void setupServer(void);
	int getServersocket(void) const;
	void run(void);
	
};

#endif
