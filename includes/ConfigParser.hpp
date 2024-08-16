#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

#include "ServerDirective.hpp"

class BlocServer;
class BlocLocation;

class ConfigParser
{
public:
	ConfigParser(const std::string &filename);
	~ConfigParser(void);
	void printServers(void);
	/* MAIN */
	void parse(void);
	std::vector<BlocServer> getServersConfig( void ) const { return _servers; }


private:
	std::map<std::string, ServerDirective> _serverDirectives;
	std::string _filename;
	
	std::vector<BlocServer> _servers;
	
	BlocServer getServerConfig(std::ifstream &file_config);
	BlocLocation getLocationConfig(std::ifstream &configFile, std::string &path);
	void checkAttribut();

	// Method init

	// Method utils
	bool isValidLineLocation(BlocLocation& location, std::vector<std::string>& tokens, std::string& key);
	bool isValidLineServer(BlocServer& server, std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile);

};

#endif // CONFIGPARSER_HPP
