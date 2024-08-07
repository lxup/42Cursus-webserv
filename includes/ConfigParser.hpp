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
	Server getServerConfig(std::ifstream &file_config);
	Location getLocationConfig(std::ifstream &configFile, std::string &path);
	~ConfigParser(void);
	void checkAttribut();

private:
	std::string _filename;
	std::vector<Server> _servers;

	/* MAIN */
	void _parse(void);
};

#endif // CONFIGPARSER_HPP
