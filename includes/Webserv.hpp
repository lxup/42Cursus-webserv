#pragma once
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

// /* LIBRARIES */
// #include <cstdlib>
// #include <fstream> // read file
// #include <sys/stat.h>
// #include <stdlib.h>
// #include <sstream>
// #include <sys/epoll.h> // pour epoll API
// #include <sys/socket.h> // For socket functions
// #include <netinet/in.h> // For sockaddr_in
// #include <iostream>		// For cout
// #include <unistd.h>		// For read
// #include <arpa/inet.h>  // pour inet_ntop
// #include <csignal> // pour signal
// #include <stdarg.h>


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

// include SIGNAL
# include <csignal>

// /* SYSTEM */
// # include <sys/stat.h>

/* CONTAINER */
// #include <vector>
// #include <map>
// #include <list>
// #include <iterator>
// #include <stack>
// #include <algorithm>
// #include <queue>


/* CUSTOM */
// #include "Defines.hpp"
// #include "Logger.hpp"
#include "ArgsManager.hpp"
// #include "Utils.hpp"
// #include "ConfigParser.hpp"
// #include "BlocLocation.hpp"
// #include "BlocServer.hpp"
#include "Server.hpp"
// #include "Socket.hpp"
// #include "ListenConfig.hpp"

extern Server g_server;

#endif // WEBSERV_HPP
