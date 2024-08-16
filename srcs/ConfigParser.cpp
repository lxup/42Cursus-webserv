#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename)
	: _filename(filename){
}

ConfigParser::~ConfigParser(void) {}

e_boolMod strToBool(std::string &str)
{
	if (str == "on")
		return (TRUE);
	else if (str == "off")
		return (FALSE);
	return (BAD);
}


/**
 * @brief This function checks if a line in the configuration file is a good directive in BlocLocation bloc (autoindex par ex)
 * It updates the location object with the corresponding values if the line is valid.
 * 
 * @param location  The BlocLocation object to update.
 * @param tokens  A vector of strings containing the tokens of the line.
 * @param key The key of the line. (the first token)
 */
bool ConfigParser::isValidLineLocation(BlocLocation& location, std::vector<std::string>& tokens, std::string& key)
{
	if (key == "root" && !tokens[1].empty())
		location.setRoot(tokens[1]);
	else if (key == "autoindex" && !tokens[1].empty())
		location.setAutoIndex(strToBool(tokens[1]));
	else if (key == "rewrite" && !tokens[1].empty())
		location.setRewrite(tokens[1]);
	else if (key == "alias" && !tokens[1].empty())
		location.setAlias(tokens[1]);
	else if (key == "allowed_methods" && !tokens[1].empty())
	{
		location.incrementCounter("allowedMethods");
		for (size_t i = 1; i < tokens.size(); i++)
			location.addAllowedMethods(tokens[i]);
	}
	else if (key == "index" && !key.empty())
	{
		location.incrementCounter("files");
		for (size_t i = 1; i < tokens.size(); i++)
			location.addFile(tokens[i]);
	}
	else
		return false;
	return true;
}


/**
 * @brief parse a BlocLocation bloc
 * 
 */
BlocLocation ConfigParser::getLocationConfig(std::ifstream &configFile, std::string &path)
{
	BlocLocation location;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseLocation = false;

	location.setLocation(path);
	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key == "}")
		{
			isCloseLocation = true;
			break;
		}
		if (isValidLineLocation(location, tokens, key))
			continue;
		else
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
	}
	if (!isCloseLocation)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename.c_str());
	
	return location;
}


/**
 * @brief This function checks if a line in the configuration file is a good directive in BlocServer bloc (server_name par ex)
 * It updates the server object with the corresponding values if the line is valid.
 * 
 * @param tokens  A vector of strings containing the tokens of the line.
 * @param key The key of the line. (the first token)
 */
bool ConfigParser::isValidLineServer(BlocServer& server, std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile){
	if (tokens.size() == 3 && key == "location" && tokens[2] == "{"){
		BlocLocation location = getLocationConfig(configFile, tokens[1]);
		location.checkDoubleLine();
		server.addLocation(location);
	}
	else if (key == "listen")
	{
		std::vector<std::string> ip = split(tokens[1], ":");
		server.setIp(ip[0]);
		server.setPort(std::atoi(ip[1].c_str()));
	}
	else if (key == "server_name")
		server.setServerName(tokens[1]);
	else if (key == "root")
		server.setRoot(tokens[1]);
	else if (key == "client_max_body_size")
		server.setClientMaxBodySize(std::atoi(tokens[1].c_str()));
	else if (key == "error_page")
	{
		server.incrementCounter(tokens[1]);
		server.addErrorPages(std::atoi(tokens[1].c_str()), tokens[2]);
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
BlocServer ConfigParser::getServerConfig(std::ifstream &configFile)
{
	BlocServer server;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseServer = false;

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key == "}"){
			isCloseServer = true;
			break;
		}
		else if (isValidLineServer(server, tokens, key, configFile))
			continue ;
		else
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
	}
	if (isCloseServer == false)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename.c_str());
	return (server);
}

/**
 * @brief main function to parse the config file
 * if encounter server bloc, call the getServerConfig function and 
 * add it to _servers vector
 * 
 */
// void ConfigParser::parse(void)
// {
// 	Logger::log(Logger::DEBUG, "Parsing config file: %s", _filename.c_str());
// 	std::ifstream configFile(_filename.c_str());
// 	if (!configFile.is_open())
// 		throw WebservException(Logger::FATAL, "File %s can't be opened or doesn't exist", _filename.c_str());
	

// }

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
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		if (tokens[0] == "server" && tokens[1] == "{"){
			BlocServer server = getServerConfig(configFile);
			server.checkDoubleLine();
			_servers.push_back(server);
		}
		else{
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
		}
	}
	checkAttribut();
	configFile.close();
}


void ConfigParser::checkAttribut()
{
	std::vector<BlocServer>::iterator it;

	for (it = _servers.begin(); it != _servers.end(); it++)
		(*it).checkAttribut();
}


void ConfigParser::printServers(void){
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "============ SERVER " << i << " ===========\n"
				  << std::endl;
		_servers[i].printServer();
	}
}



/*  */
/* --------------------------------- METHODS --------------------------------- */
/*  */

// Init
std::map<std::string, ServerDirective> ConfigParser::_generateServerDirectives(void)
{
	std::map<std::string, ServerDirective> serverDirectives;

	serverDirectives["server"] = ServerDirective(0, false, 1, 0);
	serverDirectives["listen"] = ServerDirective(1, false, 0, 0);
	serverDirectives["server_name"] = ServerDirective(1, false, 0, 0);
	serverDirectives["location"] = ServerDirective(0, false, 1, 0);

	return serverDirectives;
}