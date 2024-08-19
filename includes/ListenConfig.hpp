
#ifndef LISTENCONFIG_HPP
#define LISTENCONFIG_HPP

# include <iostream>
# include <vector>

# include "Utils.hpp"
# include "Logger.hpp"

class ListenConfig
{
private:
 	std::string _ip;
	unsigned int _port;
	std::string _ipPortJoin;

	// Checker
	bool checkIpPort();
	bool isNotInRangeIp(std::string str, int j, int i);


public:
	ListenConfig();
	ListenConfig(std::string token);
	~ListenConfig();

	// Getters
	const std::string &getIp() const { return _ip; }
	unsigned int getPort() const { return _port; }
	const std::string getIpPortJoin() const { return _ipPortJoin; }

};

#endif // LISTEN_HPP