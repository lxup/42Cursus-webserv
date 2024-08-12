/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:02:14 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/12 17:08:15 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// /* LIBRARIES */
#include <cstdlib>
#include <fstream> // read file
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
//#include <cstdlib>
// # include <iostream>
// # include <string>
// # include <fcntl.h>
// # include <exception>
// # include <cerrno>
// # include <ctime>
// # include <cstdarg>
// # include <cstring>
// # include <cstdio>

// /* SYSTEM */
// # include <sys/stat.h>

/* CONTAINER */
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <stack>
#include <algorithm>
/* CUSTOM */
#include "Defines.hpp"
#include "Logger.hpp"
#include "ArgsManager.hpp"
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include "Location.hpp"
#include "Server.hpp"
# include "SimpleServer.hpp"

#endif // WEBSERV_HPP
