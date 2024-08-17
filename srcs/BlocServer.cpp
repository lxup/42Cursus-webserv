#include "BlocServer.hpp"

// ============ GENERAL ============

BlocServer::BlocServer(std::string filename) : _clientMaxBodySize(-1), _filename(filename)
{
	_counterView["root"] = 0;
	_counterView["clientMaxBodySize"] = 0;
}

BlocServer::~BlocServer(){}

//void BlocServer::checkAttribut()
//{
//	std::vector<BlocServer>::iterator it;

//	for (it = _servers.begin(); it != _servers.end(); it++)
//		(*it).checkAttribut();
//}


// ============ UTILS ============
bool BlocServer::isStartBlocLocation(std::vector<std::string>& tokens)
{
	return (tokens.size() == 3 && tokens[0] == "location" && tokens[2] == "{");
}

void BlocServer::addListen(std::string &token)
{
	Listen listen(token);
	
	if (_listens.find(listen.getIpPortJoin()) != _listens.end())
		throw WebservException(Logger::FATAL, "Dupplicate listen in server context: %s", token.c_str());

	_listens[listen.getIpPortJoin()] = listen;
}

void BlocServer::addServerName(std::vector<std::string>& token){
	for (size_t i = 1; i < token.size(); i++){
		if (std::find(_serverNames.begin(), _serverNames.end(), token[i]) == _serverNames.end())
			_serverNames.push_back(token[i]);
	}
}

void BlocServer::addIndexes(std::vector<std::string>& token){
	for (size_t i = 1; i < token.size(); i++){
		if (std::find(_indexes.begin(), _indexes.end(), token[i]) == _indexes.end())
			_indexes.push_back(token[i]);
	}
}

void BlocServer::addErrorPages(unsigned int errorCode, std::string file)
{ 
	if (errorCode < 400 || errorCode > 599)
		throw WebservException(Logger::FATAL, "Invalid error code: %d in file %s:%d", errorCode, _filename.c_str(), ConfigParser::countLineFile);
	_errorPages[errorCode] = file; 
}

// ============ CHECKER ============
void BlocServer::checkAttribut()
{
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



void BlocServer::checkDoubleLine()
{
	std::map<std::string, int>::iterator it;

	for (it = _counterView.begin(); it != _counterView.end(); ++it)
		if (it->second > 1)
			throw WebservException(Logger::FATAL, "Dupplicate line in location context: %s", it->first.c_str());
}




// ============ PARSING ============

/**
 * @brief This function sets the default values for the server object.
 * At the all end
 * 
 */
void BlocServer::setDefaultValue()
{
	if (_listens.empty()){
		Listen listen("0.0.0.0:80");
		_listens["0.0.0.0:80"] = listen;
	}
	if (_serverNames.empty())
		_serverNames.push_back("localhost");
	if (_root.empty())
		_root = "./config/0.conf";
	if (_indexes.empty())
		_indexes.push_back("index.html");
}

/**
 * @brief This function checks if a line in the configuration file is a good directive in BlocServer bloc (server_name par ex)
 * It updates the server object with the corresponding values if the line is valid.
 * 
 * @param tokens  A vector of strings containing the tokens of the line.
 * @param key The key of the line. (the first token)
 */
bool BlocServer::isValidLineServer(std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile){
	if (tokens.size() < 2)
		return false;
	if (isStartBlocLocation(tokens)){
		BlocLocation location(_filename);
		addLocation(location.getLocationConfig(configFile, tokens[1]));
	}
	else if (key == "listen" && tokens.size() == 2)
		addListen(tokens[1]);
	else if (key == "server_name")
		addServerName(tokens);
	else if (key == "index")
		addIndexes(tokens);
	else if (key == "root" && tokens.size() == 2)
		setRoot(tokens[1]);
	else if (key == "client_max_body_size" && tokens.size() == 2)
		setClientMaxBodySize(std::atoi(tokens[1].c_str()));
	else if (key == "error_page" && tokens.size() == 3){
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
		throw WebservException(Logger::FATAL, "Missing } in file %s:%d", _filename.c_str(), ConfigParser::countLineFile);
	checkDoubleLine();
	setDefaultValue();
	return (*this);
}




// ============ PRINT ============
void BlocServer::printServer(void) const
{
	// Server names
	std::cout << "Server names: " << std::endl;
	for (size_t i = 0; i < _serverNames.size(); i++){
		std::cout << "	- " << _serverNames[i] << std::endl;
	}
	std::cout << std::endl;
	
	// Listen
	std::cout << "Listens: " << std::endl;
	for (std::map<std::string, Listen>::const_iterator it = _listens.begin(); it != _listens.end(); ++it)
		std::cout << "	- " << it->second.getIp() << ":" << it->second.getPort() << std::endl;

	// Indexes
	std::cout << "Indexes: " << std::endl;
	for (size_t i = 0; i < _indexes.size(); i++)
		std::cout << "	- " << _indexes[i] << std::endl;

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
