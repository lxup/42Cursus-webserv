#ifndef SERVER_H
#define SERVER_H

# include <iostream>
# include <map>
# include <vector>
# include <iomanip>

# include "Logger.hpp"
# include "Utils.hpp"
# include "ConfigParser.hpp"
# include "BlocLocation.hpp"
# include "Listen.hpp"

class BlocLocation;

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
	void checkDoubleLine();
	void incrementCounter(const std::string &key) { _counterView[key]++; }
	bool isValidLineServer(std::vector<std::string>& tokens, std::string& key, std::ifstream &configFile);
	bool isStartBlocLocation(std::vector<std::string>& tokens);
	void checkDoubleLocation();


public:
	BlocServer(std::string filename);
	~BlocServer();

	// parsing
	BlocServer getServerConfig(std::ifstream &file_config);

	// Getters
	const std::map<unsigned int, std::string> &getErrorPages() const { return _errorPages; }
	const std::vector<std::string> &getServerNames() const { return _serverNames; }
	const std::vector<BlocLocation> &getLocations() const { return _locations; }
	const std::string &getRoot() const { return _root; }
	unsigned int getClientMaxBodySize() const { return _clientMaxBodySize; }
	const std::map<std::string, Listen> &getListens() const { return _listens; }
	const std::vector<std::string> &getIndexes() const { return _indexes; }

	// Util
	bool isServerNamePresent(std::vector<std::string>& otherNames);


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

	// Print
	void printServer(void);
	void printListens();
	void printPair(const std::string& label, const std::string& value);
	void printInt(const std::string& label, int value);
	void printVector(const std::string& label, const std::vector<std::string>& vec);
	void printMap(const std::string& label, const std::map<unsigned int, std::string>& map);


};

#endif