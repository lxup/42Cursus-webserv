/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 14:12:27 by rgiraud           #+#    #+#             */
/*   Updated: 2024/08/02 16:31:31 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SimpleServer.hpp"

SimpleServer::SimpleServer(int port, int maxConnection) : _port(port), _maxConnection(maxConnection) {}

SimpleServer::~SimpleServer()
{
	close(_server_socket);
}

int SimpleServer::check(int ret, std::string msg)
{
	if (ret < 0)
		Logger::log(Logger::ERROR, msg.c_str());
	return ret;
}

void SimpleServer::setupServer()
{
	check((_server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Error with function socket");

	_server_address.sin_family = AF_INET;
	_server_address.sin_port = htons(_port);
	_server_address.sin_addr.s_addr = INADDR_ANY;

	check(bind(_server_socket, (const sockaddr *)(&_server_address), sizeof(_server_address)), "Error with function bind");

	check(listen(_server_socket, _maxConnection), "Error with function listen");
}

int SimpleServer::getServersocket(void) const{
	return _server_socket;
}

/**
 * @brief accept a new connection with accept() function
 * return the client socket of the new connection
 */
int SimpleServer::acceptConnection(void)
{
	_addr_size = sizeof(_server_address);
	_client_socket = accept(_server_socket, (struct sockaddr *)(&_server_address), (socklen_t *)&_addr_size);
	check(_client_socket, "Error with function accept");
	return (_client_socket);
}

void SimpleServer::showIpClient(void)
{
	char client_adress[BUFFER_SIZE + 1];
	inet_ntop(AF_INET, &_server_address, client_adress, BUFFER_SIZE);
	Logger::log(Logger::INFO, "Ip du client: %s", client_adress);
}

/**
 * @brief handle the connection with the client
 * read the message from the client
 * send a response to the client
 *
 * @warning this function is not safe (if the client send a message bigger than BUFFER_SIZE, the program will crash)
 */
void SimpleServer::handleConnection()
{

	char buffer[BUFFER_SIZE];
	ssize_t bytesRead;
	memset(&buffer, 0, BUFFER_SIZE);

	while ((bytesRead = read(_client_socket, buffer, BUFFER_SIZE - 1)) > 0)
	{
		Logger::log(Logger::INFO, "Le message fait: %d characteres", bytesRead);
		Logger::log(Logger::INFO, buffer);
		if (buffer[bytesRead - 1] == '\n')
			break;
		memset(&buffer, 0, BUFFER_SIZE);
	}

	// std::string response = "Salut du mini Server de Raf\n";
	const std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \n\n<h1>Hello, World bonjour louis!</h1>";
	check((send(_client_socket, response.c_str(), response.size(), 0)), "Error with function send");
	close(_client_socket);
	_client_socket = -1;
	showIpClient();
}

/**
 * @brief run the server and wait for connections
 * use of select() to handle multiple connections
 */
void SimpleServer::run(void)
{

	fd_set current_sockets, ready_sockets;

	FD_ZERO(&current_sockets);
	FD_SET(_server_socket, &current_sockets);

	while (true)
	{
		Logger::log(Logger::INFO, "Waiting for connections of port: %d", _port);

		// sauvegarde parce que select() va me niquer current_sockets;
		ready_sockets = current_sockets;
		
		check(select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL), "Error with function select");

		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == _server_socket)
				{
					// this is a new connection
					_client_socket = acceptConnection();
					FD_SET(_client_socket, &current_sockets);
				}
				else
				{
					// do whatever we do with connections.
					handleConnection();
					FD_CLR(i, &current_sockets);
				}
			}
		}
	}
}
