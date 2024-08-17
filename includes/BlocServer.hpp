#ifndef SERVER_H
#define SERVER_H

#include "Webserv.hpp"

class BlocLocation;

class BlocServer
{
private:
	// config bloc server
	std::string _ip;
	unsigned int _port;
	std::vector<std::string> _serverName;
	std::string _root;
	unsigned int _clientMaxBodySize;
	std::vector<BlocLocation> _locations;
	std::map<unsigned int, std::string> _errorPages;
	std::map<std::string, int> _counter;

	// divers
	std::string _filename;
	// Methods
	bool fileExistMap();
	bool checkIp();
	//void checkLocation();

public:
	BlocServer(std::string filename);
	~BlocServer();

	// Methods
	void checkAttribut();
	void checkDoubleLine();
	void incrementCounter(const std::string &key) { _counter[key]++; }

	// parsing
	BlocServer getServerConfig(std::ifstream &file_config);
	bool isValidLineServer(std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile);

	// Getters
	const std::string &getIp() const { return _ip; }
	unsigned int getPort() const { return _port; }
	const std::map<unsigned int, std::string> &getErrorPages() const { return _errorPages; }
	const std::vector<std::string> &getServerName() const { return _serverName; }
	const std::vector<BlocLocation> &getLocations() const { return _locations; }
	const std::string &getRoot() const { return _root; }
	unsigned int getClientMaxBodySize() const { return _clientMaxBodySize; }
	std::string getIpPortJoin() const;

	// Setters

	void setIp(const std::string &ip) { _ip = ip; }
	void setClientMaxBodySize(unsigned int clientMaxBodySize)
	{
		_clientMaxBodySize = clientMaxBodySize;
		_counter["clientMaxBodySize"]++;
	}
	void setRoot(const std::string &root)
	{
		_root = root;
		_counter["root"]++;
	}
	void setLocations(const std::vector<BlocLocation> &locations) { _locations = locations; }
	void setPort(unsigned int port){ _port = port; }
	void setServerName(const std::vector<std::string> &serverName){ _serverName = serverName;}
	void setErrorPages(const std::map<unsigned int, std::string> &errorPage) { _errorPages = errorPage; }

	// Adders
	void addErrorPages(unsigned int errorCode, std::string file) { _errorPages[errorCode] = file; }
	void addLocation(const BlocLocation &locations) { _locations.push_back(locations); }

	void printServer(void) const;
};

#endif