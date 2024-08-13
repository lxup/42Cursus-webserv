#ifndef UTILS_HPP
#define UTILS_HPP

#include "Webserv.hpp"

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

#endif // UTILS_HPP