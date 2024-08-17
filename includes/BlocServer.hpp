#ifndef SERVER_H
#define SERVER_H

#include "Webserv.hpp"

class BlocLocation;
class Listen;

class BlocServer
{
private:
	// config bloc server
	std::map<std::string, Listen> _listens;
	std::vector<std::string> _serverNames;
	std::vector<std::string> _indexes;
	std::string _root;
	int _clientMaxBodySize;
	std::vector<BlocLocation> _locations;
	std::map<unsigned int, std::string> _errorPages;

	// divers
	std::string _filename;
	std::map<std::string, int> _counterView;

	// Methods
	bool fileExistMap();
	//void checkLocation();

public:
	BlocServer(std::string filename);
	~BlocServer();

	// Methods
	void checkAttribut();
	void checkDoubleLine();
	void incrementCounter(const std::string &key) { _counterView[key]++; }

	// parsing
	BlocServer getServerConfig(std::ifstream &file_config);
	bool isValidLineServer(std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile);

	// utils
	bool isStartBlocLocation(std::vector<std::string>& tokens);


	// Getters
	const std::map<unsigned int, std::string> &getErrorPages() const { return _errorPages; }
	const std::vector<std::string> &getServerName() const { return _serverNames; }
	const std::vector<BlocLocation> &getLocations() const { return _locations; }
	const std::string &getRoot() const { return _root; }
	unsigned int getClientMaxBodySize() const { return _clientMaxBodySize; }
	const std::map<std::string, Listen> &getListens() const { return _listens; }
	const std::vector<std::string> &getIndexes() const { return _indexes; }

	// Setters
	void setClientMaxBodySize(unsigned int clientMaxBodySize)
	{
		_clientMaxBodySize = clientMaxBodySize;
		_counterView["clientMaxBodySize"]++;
	}
	void setRoot(const std::string &root)
	{
		_root = root;
		_counterView["root"]++;
	}
	void setDefaultValue();
	void setLocations(const std::vector<BlocLocation> &locations) { _locations = locations; }
	void setErrorPages(const std::map<unsigned int, std::string> &errorPage) { _errorPages = errorPage; }

	// Adders
	void addErrorPages(unsigned int errorCode, std::string file);
	void addLocation(const BlocLocation &locations) { _locations.push_back(locations); }
	void addListen(std::string &token);
	void addServerName(std::vector<std::string>& token);
	void addIndexes(std::vector<std::string>& token);

	void printServer(void) const;
};

#endif