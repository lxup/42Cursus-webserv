/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:02:14 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/13 12:16:20 by marvin           ###   ########.fr       */
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
#include <sys/epoll.h> // pour epoll API
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <arpa/inet.h>  // pour inet_ntop


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
#include "BlocLocation.hpp"
#include "BlocServer.hpp"
#include "Server.hpp"

#endif // WEBSERV_HPP
