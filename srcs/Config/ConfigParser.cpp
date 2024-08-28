#include "ConfigParser.hpp"

// ConfigParser::ConfigParser(const std::string &filename) : _filename(filename){
// }

std::vector<std::string> ConfigParser::supportedMethods = ConfigParser::_getSupportedMethods();
std::vector<std::string> ConfigParser::supportedHttpVersions = ConfigParser::_getSupportedHttpVersions();


ConfigParser::ConfigParser(void) : _filename("") {}

ConfigParser::~ConfigParser(void) {}

// ================== UTILS ==================

bool ConfigParser::isStartBlocServer(std::vector<std::string> tokens){
	return ((tokens[0] == "server" && tokens[1] == "{") && tokens.size() == 2) 
				|| (tokens[0] == "server{" && tokens.size() == 1);
}


/**
 * fonction qui check si il n'y a pas deux bloc server avec 
 * le meme serverName
*/
void ConfigParser::checkDoubleServerName(){
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < _servers.size(); i++){
		std::vector<std::string> currNames = _servers[i].getServerNames();
		for (size_t j = i + 1; j < _servers.size(); j++){
			if (_servers[j].isServerNamePresent(currNames))
				Logger::log(Logger::FATAL, "conflicting server name \"%s\" on %s", currNames[0].c_str(), currNames[1].c_str());
		}
	}
}

/**
 * fait les groupe de serverBloc par ip:port
*/
void ConfigParser::assignConfigs(){
	
	for (size_t i = 0; i < _servers.size(); i++){
		std::map<std::string, ListenConfig> listens = _servers[i].getListens();
		for (std::map<std::string, ListenConfig>::iterator it = listens.begin(); it != listens.end() ; ++it){
			_configs[it->first].push_back(_servers[i]);
		}	
	}
	
}

// ================== PARSER ==================
/**
 * @brief main function to parse the config file
 * if encounter server bloc, call the getServerConfig function and 
 * add it to _servers vector
 * 
 */
void ConfigParser::parse(const std::string &filename)
{
	this->_filename = filename;
	Logger::log(Logger::DEBUG, "Parsing config file: %s", _filename.c_str());
	std::ifstream configFile(_filename.c_str());
	std::vector<std::string> tokens;
	std::string line;

	if (!configFile.is_open())
		Logger::log(Logger::FATAL, "File %s can't be opened or doesn't exist", _filename.c_str());
	while (std::getline(configFile, line))
	{
		ConfigParser::countLineFile++;
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		if (isStartBlocServer(tokens)){
			BlocServer server(_filename);
			_servers.push_back(server.getServerConfig(configFile));
		}
		else
			Logger::log(Logger::FATAL, "Invalid line: \"%s\" in file: %s:%d", line.c_str(), _filename.c_str(), ConfigParser::countLineFile);
	}
	// add Default server if no server bloc
	if (_servers.size() == 0){
		BlocServer server(_filename);
		_servers.push_back(server.getServerConfig(configFile));
	}
	checkDoubleServerName();
	assignConfigs();
	configFile.close();
}


// ============ PRINT ============
void ConfigParser::printServers(void){
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "============ SERVER " << i + 1 << " ===========\n"
				  << std::endl;
		_servers[i].printServer();
		std::cout << std::endl;
	}
}

/*
** --------------------------------- STATIC ---------------------------------
*/

/**
 * @brief Get the supported methods object
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string>	ConfigParser::_getSupportedMethods(void)
{
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	return (methods);
}

/**
 * @brief Check if the method is supported
 * 
 * @param method 
 * @return true 
 * @return false 
 */
bool	ConfigParser::isMethodSupported(std::string method)
{
	return (std::find(ConfigParser::supportedMethods.begin(), ConfigParser::supportedMethods.end(), method) != ConfigParser::supportedMethods.end());
}

/**
 * @brief Get the supported Http Versions object
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string>	ConfigParser::_getSupportedHttpVersions(void)
{
	std::vector<std::string> versions;
	versions.push_back("HTTP/1.0");
	versions.push_back("HTTP/1.1");
	return (versions);
}

/**
 * @brief Check if the http version is supported
 * 
 * @param version 
 * @return true 
 * @return false 
 */
bool	ConfigParser::isHttpVersionSupported(std::string version)
{
	return (std::find(ConfigParser::supportedHttpVersions.begin(), ConfigParser::supportedHttpVersions.end(), version) != ConfigParser::supportedHttpVersions.end());
}
