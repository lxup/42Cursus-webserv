/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:02:14 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/26 02:16:27 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* LIBRARIES */
# include <iostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <exception>

/* SYSTEM */
# include <sys/stat.h>

/* CONTAINER */
# include <vector>
# include <map>
# include <list>
# include <iterator>

/* CUSTOM */
# include "Defines.hpp"
# include "Logger.hpp"
# include "InputArgs.hpp"

/* UTILS */
void	printMsg(std::ostream &os, const char *msg, ...);

#endif // WEBSERV_HPP
