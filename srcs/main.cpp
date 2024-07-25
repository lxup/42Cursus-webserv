/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:08:46 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/25 22:14:49 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	Logger::setLogFileState(true);
	std::string fileName = Logger::getLogFileName();
	std::cout << "File name : " << fileName << std::endl;

	Logger::log(Logger::FATAL, "This is a test %s", "FATAL");

	return (0);
}
