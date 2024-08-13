#include "Server.hpp"

Server::Server() : _port(69), _serverName(""), _root("./config/0.conf"), _clientMaxBodySize(0)
{
	_counter["port"] = 0;
	_counter["serverName"] = 0;
	_counter["root"] = 0;
	_counter["errorPage"] = 0;
	_counter["clientMaxBodySize"] = 0;
}

Server::~Server()
{
}

void Server::checkAttribut()
{
	if (getPort() == 0)
		throw WebservException(Logger::FATAL, "Invalid Port value");
	if (!directoryExist(_root.c_str()))
		throw WebservException(Logger::FATAL, "Invalid Root value");
	if (!fileExistMap())
		throw WebservException(Logger::FATAL, "Invalid Error value");
	checkLocation();
}

void Server::checkLocation()
{
	std::vector<Location>::iterator itLoc;
	int i = 0;

	for (itLoc = _locations.begin(); itLoc != _locations.end(); itLoc++)
	{
		std::cout << i++ << std::endl;
		(*itLoc).checkValue();
	}
}

bool Server::fileExistMap()
{
	std::map<unsigned int, std::string>::const_iterator it;

	for (it = getErrorPages().begin(); it != getErrorPages().end(); ++it)
		if (!fileExist(it->second))
			return (false);
	return (true);
}

void Server::printServer(void) const
{
	std::cout << "Server name: " << _serverName << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Client max body size: " << _clientMaxBodySize << std::endl;
	std::cout << "Error pages: " << std::endl

		;
	for (std::map<unsigned int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
		std::cout << it->first << " => " << it->second << std::endl;
	std::cout << std::endl;

	int i = 0;
	for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		std::cout << "\n-- LOCATION " << i++ << " --" << std::endl;
		it->printLocation();
	}
}

void Server::checkDoubleLine()
{

	std::map<std::string, int>::iterator it;

	for (it = _counter.begin(); it != _counter.end(); ++it)
	{
		if (it->second > 1)
		{
			throw WebservException(Logger::FATAL, "Dupplicate line in location context: %s", it->first.c_str());
		}
	}
}
