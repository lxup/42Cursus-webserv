
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

#include "Logger.hpp"

/* WEBSERV EXCEPTION */
class WebservException : public std::exception
{
private:
	int _errno;
	Logger::LogLevel _logLevel;
	std::string _msg;

public:
	WebservException(int errnoNum, const char *msg, ...);
	WebservException(Logger::LogLevel logLevel, const char *msg, ...);
	virtual ~WebservException(void) throw() {}
	virtual const char *what() const throw() { return _msg.c_str(); }
	Logger::LogLevel getLogLevel(void) const { return _logLevel; }
};

/* UTILS */
void printMsg(std::ostream &os, const char *msg, ...);
bool directoryExist(const char *path);
bool fileExist(const std::string &name);
std::string trimLine(std::string &line);
std::vector<std::string> split(std::string s, std::string delimiter);
std::string unsignedIntToString(unsigned int value);
std::string intToString(int value);

int	protectedCall(int ret, std::string msg, bool isFatal = true);

#endif // UTILS_HPP