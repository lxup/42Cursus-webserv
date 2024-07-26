/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 12:28:03 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/26 19:07:08 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Logger.hpp"

# include <iostream>

class ConfigParser
{
	public:
		ConfigParser(const std::string &filename);
		~ConfigParser(void);

	private:
		std::string		_filename;

		/* MAIN */
		void			_parse(void);
};


#endif // CONFIGPARSER_HPP
