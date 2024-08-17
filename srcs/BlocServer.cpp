#include "BlocServer.hpp"

// ============ GENERAL ============

BlocServer::BlocServer(std::string filename) : _port(69), _root("./config/0.conf"), _clientMaxBodySize(0), _filename(filename)
{
	_serverName.push_back("localhost");
	_counter["root"] = 0;
	_counter["clientMaxBodySize"] = 0;
}

BlocServer::~BlocServer(){}


std::string BlocServer::getIpPortJoin() const{
	return (_ip + ":" + unsignedIntToString(_port));
}


//void BlocServer::checkAttribut()
//{
//	std::vector<BlocServer>::iterator it;

//	for (it = _servers.begin(); it != _servers.end(); it++)
//		(*it).checkAttribut();
//}


// ============ CHECKER ============
void BlocServer::checkAttribut()
{
	if (!checkIp())
		throw WebservException(Logger::FATAL, "Invalid Ip/Port value");
	//checkLocation();
}

//void BlocServer::checkLocation()
//{
//	std::vector<BlocLocation>::iterator itLoc;

//	for (itLoc = _locations.begin(); itLoc != _locations.end(); itLoc++)
//		(*itLoc).checkValue();
//}

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


void BlocServer::checkDoubleLine()
{
	std::map<std::string, int>::iterator it;

	for (it = _counter.begin(); it != _counter.end(); ++it)
		if (it->second > 1)
			throw WebservException(Logger::FATAL, "Dupplicate line in location context: %s", it->first.c_str());
}




// ============ PARSING ============

/**
 * @brief This function checks if a line in the configuration file is a good directive in BlocServer bloc (server_name par ex)
 * It updates the server object with the corresponding values if the line is valid.
 * 
 * @param tokens  A vector of strings containing the tokens of the line.
 * @param key The key of the line. (the first token)
 */
bool BlocServer::isValidLineServer(std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile){
	if (tokens.size() == 3 && key == "location" && tokens[2] == "{"){
		BlocLocation location(_filename);
		addLocation(location.getLocationConfig(configFile, tokens[1]));
	}
	else if (key == "listen")
	{
		std::vector<std::string> ip = split(tokens[1], ":");
		setIp(ip[0]);
		setPort(std::atoi(ip[1].c_str()));
	}
	else if (key == "server_name")
		setServerName(split(tokens[1], " "));
	else if (key == "root")
		setRoot(tokens[1]);
	else if (key == "client_max_body_size")
		setClientMaxBodySize(std::atoi(tokens[1].c_str()));
	else if (key == "error_page")
	{
		incrementCounter(tokens[1]);
		addErrorPages(std::atoi(tokens[1].c_str()), tokens[2]);
	}else
		return (false);
	return (true);
}


/**
 * @brief parse a server bloc and if encounter a location bloc, it call getLocationConfig() to parse it
 * 
 * @param configFile 
 * @return BlocServer 
 */
BlocServer BlocServer::getServerConfig(std::ifstream &configFile)
{
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseServer = false;

	while (std::getline(configFile, line)){
		ConfigParser::countLineFile++;
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key[0] == '}'){
			isCloseServer = true;
			break;
		}
		else if (isValidLineServer(tokens, key, configFile))
			continue ;
		else
			throw WebservException(Logger::FATAL, "Invalid line: \"%s\" in file: %s:%d", line.c_str(), _filename.c_str(), ConfigParser::countLineFile);
	}
	if (isCloseServer == false)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename);
	return (*this);
}










// ============ PRINT ============
void BlocServer::printServer(void) const
{
	std::cout << "Server names: ";
	for (size_t i = 0; i < _serverName.size(); i++)
	{
		std::cout << _serverName[i] << " ";
	}
	std::cout << std::endl;
	
	std::cout << "Ip: " << _ip << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Client max body size: " << _clientMaxBodySize << std::endl;
	std::cout << "Error pages: " << std::endl;
	for (std::map<unsigned int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
		std::cout  << "	- " << it->first << ": " << it->second << std::endl;
	std::cout << std::endl;

	int i = 0;
	for (std::vector<BlocLocation>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		std::cout << "\n-- LOCATION " << i++ << " --" << std::endl;
		it->printLocation();
	}
}
