#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

class Server;
class Location;

enum e_context
{
	SERVER,
	LOCATION
};

class ConfigParser
{
public:
	ConfigParser(const std::string &filename);
	~ConfigParser(void);
	void printServers(void);

private:
	std::string _filename;
	std::vector<Server> _servers;
	
	Server getServerConfig(std::ifstream &file_config);
	Location getLocationConfig(std::ifstream &configFile, std::string &path);
	void checkAttribut();

	/* MAIN */
	void _parse(void);
	
	// Method utils
	bool isValidLineLocation(Location& location, std::vector<std::string>& tokens, std::string& key);
	bool isValidLineServer(Server& server, std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile);

};

#endif // CONFIGPARSER_HPP
