#include "BlocServer.hpp"

BlocServer::BlocServer() : _port(69), _serverName(""), _root("./config/0.conf"), _clientMaxBodySize(0)
{
	_counter["port"] = 0;
	_counter["serverName"] = 0;
	_counter["root"] = 0;
	_counter["errorPage"] = 0;
	_counter["clientMaxBodySize"] = 0;
}

BlocServer::~BlocServer()
{
}

void BlocServer::checkAttribut()
{
	if (!checkIp())
		throw WebservException(Logger::FATAL, "Invalid Ip/Port value");
	checkLocation();
}

void BlocServer::checkLocation()
{
	std::vector<BlocLocation>::iterator itLoc;

	for (itLoc = _locations.begin(); itLoc != _locations.end(); itLoc++)
		(*itLoc).checkValue();
}

bool BlocServer::fileExistMap()
{
	std::map<unsigned int, std::string>::const_iterator it;

	for (it = getErrorPages().begin(); it != getErrorPages().end(); ++it)
		if (!fileExist(it->second))
			return (false);
	return (true);
}

bool BlocServer::checkIp()
{
	int	i = 0;
	int	j = 0;
	std::string str = _ip + ":" + unsignedIntToString(_port);

	for (int p = 0; p < 3; p++)
	{
		while (isdigit(str[i + j]))
		i++;
		if (i == 0 || i > 3 || str[j + i++] != '.')
			return (false);
		j += i;
		i = 0;
	}
	while (isdigit(str[i + j]))
		i++;
	if (i == 0 || i > 3 || str[j + i++] != ':')
		return (false);
	if (_port > 65535)
		return (false);
	return (true);
}

void BlocServer::printServer(void) const
{
	std::cout << "Server name: " << _serverName << std::endl;
	std::cout << "Ip: " << _ip << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Client max body size: " << _clientMaxBodySize << std::endl;
	std::cout << "Error pages: " << std::endl;
	for (std::map<unsigned int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
		std::cout << it->first << "	- " << it->second << std::endl;
	std::cout << std::endl;

	int i = 0;
	for (std::vector<BlocLocation>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		std::cout << "\n-- LOCATION " << i++ << " --" << std::endl;
		it->printLocation();
	}
}

void BlocServer::checkDoubleLine()
{
	std::map<std::string, int>::iterator it;

	for (it = _counter.begin(); it != _counter.end(); ++it)
		if (it->second > 1)
			throw WebservException(Logger::FATAL, "Dupplicate line in location context: %s", it->first.c_str());
}

std::string BlocServer::getIpPortJoin() const{
	return (_ip + ":" + unsignedIntToString(_port));
}
