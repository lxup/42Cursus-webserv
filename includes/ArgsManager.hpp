#ifndef ARGSMANAGER_HPP
#define ARGSMANAGER_HPP

// # include "Webserv.hpp"
#include "Utils.hpp"
#include <iostream>
#include <map>

/* DEFAULT PARAMETERS */
#define DEFAULT_CONFIG_FILE_PATH "./config/default.conf"

class ArgsManager
{
private:
	int _ac;
	char **_av;
	std::string _configFilePath;
	std::map<std::string, bool> _options;
	int _state;

	/* UTILS */
	void _parse(void);
	std::map<std::string, bool> _generateOptions(void);
	std::string _convertToLongOption(std::string option);
	std::string _convertToShortOption(std::string option);

public:
	ArgsManager(int argc, char **argv);
	~ArgsManager(void);

	bool isOption(std::string option);
	void help(void);
	void summary(void);

	/* GETTERS */
	int getState(void) const;
	std::string getConfigFilePath(void) const;
};

#endif // ARGSMANAGER_HPP
