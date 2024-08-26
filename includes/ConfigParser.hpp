#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <string>

# include "Utils.hpp"
# include "BlocServer.hpp"

class BlocServer;

class ConfigParser
{
	public:
		static int countLineFile;

		// general
		// ConfigParser(const std::string &filename);
		ConfigParser(void);
		~ConfigParser(void);
		// std::vector<BlocServer> getServersConfig( void ) const { return _servers; }
		std::map<std::string, std::vector<BlocServer> > getConfigs( void ) const { return _configs; }
		std::map<std::string, std::vector<BlocServer> > &getServers( void ) { return _configs; }
		// parser
		void parse(const std::string &filename);

		// utils
		void checkDoubleServerName();
		bool isStartBlocServer(std::vector<std::string> tokens);
		void assignConfigs();

		// print
		void printServers(void);

		/* STATIC */
		static	std::vector<std::string>	supportedMethods;
		static	bool						isMethodSupported(std::string method);
		static	std::vector<std::string>	supportedHttpVersions;
		static	bool						isHttpVersionSupported(std::string version);
	private:
		std::string _filename;

		std::vector<BlocServer> _servers;
		std::map<std::string, std::vector<BlocServer> > _configs;

		/* STATIC */
		static std::vector<std::string>	_getSupportedMethods(void);
		static std::vector<std::string>	_getSupportedHttpVersions(void);
};

#endif // CONFIGPARSER_HPP
