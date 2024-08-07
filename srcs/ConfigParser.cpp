/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbehr <lbehrgiraud@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 11:46:55 by rgiraud           #+#    #+#             */
/*   Updated: 2024/08/06 16:28:25 by lbehr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename)
	: _filename(filename)
{
	_parse();
}

ConfigParser::~ConfigParser(void)
{
}

e_boolMod strToBool(std::string &str)
{
	if (str == "on")
		return (TRUE);
	else if (str == "off")
		return (FALSE);
	return (BAD);
}

std::string trimLine(std::string &line)
{
	const std::string white_chars = " \t\n\r\f\v";
	std::string result;

	int start = 0;
	while (white_chars.find(line[start]) != std::string::npos)
		start++;
	int end = line.size() - 1;
	while (end >= 0 && white_chars.find(line[end]) != std::string::npos)
		end--;

	for (int i = start; i <= end; i++)
		result.push_back(line[i]);

	return (result);
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> result;
	s = trimLine(s);
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
		s = trimLine(s);
	}
	result.push_back(s);
	return (result);
}

// std::vector<std::string> &addFile(std::vector<std::string> &token)
// {
// 	int	i = 0;
// 	std::vector<std::string> str;

// 	while (!token[i].empty())
// 		str[i] = token[i];
// 	return (str);
// }

Location ConfigParser::getLocationConfig(std::ifstream &configFile, std::string &path)
{
	Location location;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;

	// todo parser le path parametre
	(void)path;

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key == "root" && !tokens[1].empty())
			location.setRoot(tokens[1]);
		else if (key == "autoindex" && !tokens[1].empty())
			location.setAutoIndex(strToBool(tokens[1]));
		else if (key == "rewrite" && !tokens[1].empty())
			location.setRewrite(tokens[1]);
		else if (key == "allowedMethods" && !tokens[1].empty()){
			for (size_t i = 1; i < tokens.size(); i++)
				location.addAllowedMethods(tokens[i]);
		}
		else if (key == "index" && !key.empty()){
			for (size_t i = 1; i < tokens.size(); i++)
				location.addFile(tokens[i]);
		}
	}
	return (location);
}


Server ConfigParser::getServerConfig(std::ifstream &configFile)
{
	Server server;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key.empty())
			continue ;
		if (tokens.size() == 3 && key == "location" && tokens[2] == "{" )
			server.addLocation(getLocationConfig(configFile, tokens[1]));
		else if (key == "listen")
			server.setPort(std::atoi(tokens[1].c_str()));
		else if (key == "server_name")
			server.setServerName(tokens[1]);
		else if (key == "root")
			server.setRoot(tokens[1]);
		else if (key == "client_max_body_size")
			server.setClientMaxBodySize(std::atoi(tokens[1].c_str()));
		else if (key == "error_page")
			server.addErrorPages(std::atoi(tokens[1].c_str()), tokens[2]);
	}

	return (server);
}


void ConfigParser::_parse(void)
{
	Logger::log(Logger::DEBUG, "Parsing config file: %s", _filename.c_str());
	std::ifstream configFile(_filename.c_str());
	std::vector<std::string> tokens;
	std::string line;
	std::stack<e_context> context;

	if (!configFile.is_open())
	{
		Logger::log(Logger::ERROR, "The config file can't be open");
		exit(EXIT_FAILURE); // A MODIFIER
	}

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		if (tokens[0] == "server" && tokens[1] == "{")
		{
			if (!context.empty())
				throw WebservException(0, "Invalid line: %s", line.c_str());
			context.push(SERVER);
			_servers.push_back(getServerConfig(configFile));
		}
	}

	configFile.close();

	printf("Servers:\n");
	for (size_t i = 0; i < _servers.size(); i++)
	{
		printf("Server %lu:\n", i);
		for (size_t i = 0; i < _servers.size(); i++)
			_servers[i].printServer();
	}

}
