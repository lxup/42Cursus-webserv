
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cstdarg>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "Logger.hpp"
#include "ConfigParser.hpp"

/* UTILS */
void printMsg(std::ostream &os, const char *msg, ...);
bool directoryExist(const char *path);
bool fileExist(const std::string &name);
std::string trimLine(std::string &line);
std::vector<std::string> split(std::string s, std::string delimiter);
std::string unsignedIntToString(unsigned int value);
std::string intToString(int value);

int protectedCall(int ret, std::string msg, bool isFatal = true);

std::string extractIp(std::string ipPort);
unsigned int extractPort(std::string ipPort);
bool isEmptyFile();

// epoll utils
void addSocketEpoll(int epollFD, int sockFD, uint32_t flags);
void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags);
void deleteSocketEpoll(int epollFD, int sockFD);

#endif // UTILS_HPP