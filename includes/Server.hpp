/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 10:28:00 by rgiraud           #+#    #+#             */
/*   Updated: 2024/08/07 12:25:39 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include "Logger.hpp"
#include "Webserv.hpp"
#include "Location.hpp"

class Location;

class Server
{
private:
	unsigned int _port;
	std::string _server_name;
	std::string _root;
	unsigned int _client_max_body_size;
	std::vector<Location> _locations;
	std::map<unsigned int, std::string> _error_pages;

public:
	Server();
	~Server();

	unsigned int getPort() const { return _port; }
	const std::map<unsigned int, std::string> &getErrorPages() const { return _error_pages; }
	const std::string &getServerName() const { return _server_name; }
	const std::vector<Location> &getLocations() const { return _locations; }
	const std::string &getRoot() const { return _root; }
	unsigned int getClientMaxBodySize() const { return _client_max_body_size; }

	void setClientMaxBodySize(unsigned int client_max_body_size) { _client_max_body_size = client_max_body_size; }
	void setRoot(const std::string &root) { _root = root; }
	void setLocations(const std::vector<Location> &locations) { _locations = locations; }
	void addLocation(const Location &locations) { _locations.push_back(locations); }
	void setPort(unsigned int port) { _port = port; }
	void setServerName(const std::string &server_name) { _server_name = server_name; }
	void setErrorPages(const std::map<unsigned int, std::string> &error_pages) { _error_pages = error_pages; }
	void addErrorPages(unsigned int errorCode, std::string file) { _error_pages[errorCode] = file; }

	void printServer(void) const;
};

#endif