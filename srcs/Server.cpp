/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 10:54:48 by lbehr             #+#    #+#             */
/*   Updated: 2024/08/07 12:25:55 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _port(0), _server_name(""), _root(""), _client_max_body_size(0)
{
	(void)_port;
	(void)_server_name;
	(void)_root;
	(void)_client_max_body_size;
	(void)_locations;
	(void)_error_pages;
}

Server::~Server()
{
}

void Server::printServer(void) const
{
	std::cout << "Server name: " << _server_name << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Client max body size: " << _client_max_body_size << std::endl;
	std::cout << "Error pages: " << std::endl

		;
	for (std::map<unsigned int, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it)
		std::cout << it->first << " => " << it->second << std::endl;
	std::cout << std::endl;

	int i = 0;
	for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		std::cout << "\n-- LOCATION " << i++ << " --" << std::endl;
		it->printLocation();
	}
}
