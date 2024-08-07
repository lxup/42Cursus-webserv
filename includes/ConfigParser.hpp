/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 12:28:03 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/06 18:04:04 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"

class Server;
class Location;

enum e_context{
	SERVER,
	LOCATION
};

class ConfigParser
{
	public:
		ConfigParser(const std::string &filename);
		Server getServerConfig(std::ifstream &file_config);
		Location getLocationConfig(std::ifstream &configFile, std::string &path);
		~ConfigParser(void);

	private:
		std::string _filename;
		std::vector<Server> _servers;

		/* MAIN */
		void _parse(void);
};

#endif // CONFIGPARSER_HPP
