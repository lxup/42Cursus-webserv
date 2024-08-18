#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

class BlocServer;
class BlocLocation;

class ConfigParser
{
public:
	static int countLineFile;

	// general
	ConfigParser(const std::string &filename);
	~ConfigParser(void);
	std::vector<BlocServer> getServersConfig( void ) const { return _servers; }
	
	// parser
	void parse(void);

	// utils
	void checkDoubleServerName();
	bool isStartBlocServer(std::vector<std::string> tokens);
	void assignConfigs();

	// print
	void printServers(void);

private:
	std::string _filename;

	std::vector<BlocServer> _servers;
	std::map<std::string, std::vector<BlocServer> > _configs;
};

#endif // CONFIGPARSER_HPP
