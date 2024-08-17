#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

class BlocServer;
class BlocLocation;

class ConfigParser
{
public:
	ConfigParser(const std::string &filename);
	~ConfigParser(void);
	
	void printServers(void);
	void parse(void);
	std::vector<BlocServer> getServersConfig( void ) const { return _servers; }
	static int countLineFile;


private:
	std::string _filename;
	std::vector<BlocServer> _servers;
};

#endif // CONFIGPARSER_HPP
