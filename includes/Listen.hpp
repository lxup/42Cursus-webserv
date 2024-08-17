
#ifndef LISTEN_HPP
#define LISTEN_HPP

#include "Webserv.hpp"

class Listen
{
private:
 	std::string _ip;
	unsigned int _port;
	std::string _ipPortJoin;

	// Checker
	bool checkIpPort();

public:
	Listen();
	Listen(std::string token);
	~Listen();

	// Getters
	const std::string &getIp() const { return _ip; }
	unsigned int getPort() const { return _port; }
	const std::string getIpPortJoin() const { return _ipPortJoin; }

};

#endif // LISTEN_HPP