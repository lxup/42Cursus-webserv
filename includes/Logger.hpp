#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Defines.hpp"
#include <map>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <vector>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>

/* DEFAULT PARAMETERS */
#define DEFAULT_LOG_STATE true
#define DEFAULT_LOG_FILE_STATE false
#define DEFAULT_LOG_DEBUG_STATE false

class Logger
{
public:
	/* ENUMs */
	enum LogLevel
	{
		FATAL = 0,
		ERROR,
		WARNING,
		INFO,
		DEBUG,
	};

	/* MAIN */
	static void log(LogLevel level, const char *msg, ...);

	/* SETTERS */
	static void setLogState(bool state);
	static void setLogFileState(bool state);
	static void setLogDebugState(bool state);

	/* GETTERS */
	static bool getLogState(void);
	static bool getLogFileState(void);
	static bool getLogDebugState(void);
	static std::string getLogFileName(void);
	static std::string getLogLevelStr(LogLevel level);
	static std::string getLogLevelColor(LogLevel level);

private:
	static bool _logState;
	static bool _logFileState;
	static bool _logDebugState;
	static std::string _logFileName;
	static std::map<Logger::LogLevel, std::string> _logLevelStr;
	static std::map<Logger::LogLevel, std::string> _logLevelColor;

	/* UTILS */
	static std::string _generateLogFileName(void);
	static std::map<Logger::LogLevel, std::string> _generateLogLevelStr(void);
	static std::map<Logger::LogLevel, std::string> _generateLogLevelColor(void);
	static std::string _formater(Logger::LogLevel level, const char *msg, std::string time, bool colored = true);
	static void _printLog(Logger::LogLevel level, const char *msg, std::string time);
	static void _writeLogInFile(LogLevel level, const char *msg, std::string time);
};

#endif // LOGGER_HPP
