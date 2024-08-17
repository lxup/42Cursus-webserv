#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename) : _filename(filename){
}

ConfigParser::~ConfigParser(void) {}

// ================== UTILS ==================

bool ConfigParser::isStartBlocServer(std::vector<std::string> tokens){
	return ((tokens[0] == "server" && tokens[1] == "{") && tokens.size() == 2) 
				|| (tokens[0] == "server{" && tokens.size() == 1);
}


// ================== PARSER ==================
/**
 * @brief main function to parse the config file
 * if encounter server bloc, call the getServerConfig function and 
 * add it to _servers vector
 * 
 */
void ConfigParser::parse(void)
{
	Logger::log(Logger::DEBUG, "Parsing config file: %s", _filename.c_str());
	std::ifstream configFile(_filename.c_str());
	std::vector<std::string> tokens;
	std::string line;

	if (!configFile.is_open())
		throw WebservException(Logger::FATAL, "File %s can't be opened or doesn't exist", _filename.c_str());

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
		else{
			throw WebservException(Logger::FATAL, "Invalid line: \"%s\" in file: %s:%d", line.c_str(), _filename.c_str(), ConfigParser::countLineFile);
		}
	}
	configFile.close();
}


// ============ PRINT ============
void ConfigParser::printServers(void){
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "============ SERVER " << i << " ===========\n"
				  << std::endl;
		_servers[i].printServer();
		std::cout << std::endl;
	}
}
