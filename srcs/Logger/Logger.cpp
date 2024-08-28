#include "Logger.hpp"
#include <stdarg.h>
/*  */
/* ---------------------------- STATIC VARIABLE ---------------------------- */
/*  */

/*
 * @brief The state of the logger
 *
 * ON : The logger is on
 * OFF : The logger is off
 */
bool Logger::_logState = DEFAULT_LOG_STATE;

/*
 * @brief The state of the file logger
 *
 * true : The logger save the logs in a file
 *
 * false : The logger doesn't save the logs in a file
 */
bool Logger::_logFileState = DEFAULT_LOG_FILE_STATE;

/*
 * @brief The state of the debug logger
 *
 * true : The logger save the debug logs
 *
 * false : The logger doesn't save the debug logs
 */
bool Logger::_logDebugState = DEFAULT_LOG_DEBUG_STATE;

/*
 * @brief The file name
 */
std::string Logger::_logFileName = Logger::_generateLogFileName();

/*
 * @brief The string representation of the log levels
 */
std::map<Logger::LogLevel, std::string> Logger::_logLevelStr = Logger::_generateLogLevelStr();

/*
 * @brief The color representation of the log levels
 */
std::map<Logger::LogLevel, std::string> Logger::_logLevelColor = Logger::_generateLogLevelColor();

/*  */
/* ---------------------------------- UTILS --------------------------------- */
/*  */

/*
 * @brief Generate a log file name with the current date
 *
 * @return std::string : The generated log file name
 */
std::string Logger::_generateLogFileName(void)
{
	std::time_t t = std::time(NULL);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "webserv_%Y-%m-%d_%H-%M-%S.log", std::localtime(&t));
	return (std::string(buffer));
}

/*
 * @brief Generate the string representation of the log levels
 *
 * @return std::map<Logger::LogLevel, std::string> : The string representation of the log levels
 */
std::map<Logger::LogLevel, std::string> Logger::_generateLogLevelStr(void)
{
	std::map<Logger::LogLevel, std::string> logLevelStr;

	logLevelStr[Logger::FATAL] = "FATAL";
	logLevelStr[Logger::ERROR] = "ERROR";
	logLevelStr[Logger::WARNING] = "WARNING";
	logLevelStr[Logger::INFO] = "INFO";
	logLevelStr[Logger::TRACE] = "TRACE";
	logLevelStr[Logger::DEBUG] = "DEBUG";

	return (logLevelStr);
}

/*
 * @brief Generate the color representation of the log levels
 *
 * @return std::map<Logger::LogLevel, std::string> : The color representation of the log levels
 */
std::map<Logger::LogLevel, std::string> Logger::_generateLogLevelColor(void)
{
	std::map<Logger::LogLevel, std::string> logLevelColor;

	logLevelColor[Logger::FATAL] = C_RED;
	logLevelColor[Logger::ERROR] = C_RED;
	logLevelColor[Logger::WARNING] = C_YELLOW;
	logLevelColor[Logger::INFO] = C_GREEN;
	logLevelColor[Logger::TRACE] = C_MAGENTA;
	logLevelColor[Logger::DEBUG] = C_CYAN;

	return (logLevelColor);
}

/*
 * @brief Format a log message
 *
 * @param level The level of the message
 * @param msg The message to log
 * @param time The time of the message
 *
 * @return std::string : The formated message
 */
std::string Logger::_formater(Logger::LogLevel level, const char *msg, std::string time, bool colored)
{
	std::string formatedMsg;

	if (colored == true)
		formatedMsg += Logger::getLogLevelColor(level);
	formatedMsg += "[" + Logger::getLogLevelStr(level) + "]\t";
	formatedMsg += time + " : " + msg;
	if ((level == Logger::ERROR || level == Logger::FATAL) && errno != 0)
		formatedMsg += ": " + static_cast<std::string>(std::strerror(errno)); // TODO : FIX ERNO HERE
	if (colored == true)
		formatedMsg += C_RESET;

	return (formatedMsg);
}

/*
 * @brief Print a log message
 *
 * @param level The level of the message
 * @param time The time of the message
 * @param msg The message to log
 */
void Logger::_printLog(Logger::LogLevel level, const char *msg, std::string time)
{
	std::cout << Logger::_formater(level, msg, time) << std::endl;
}

/*
 * @brief Write a log message in a file
 *
 * @param fd The file descriptor
 * @param level The level of the message
 * @param msg The message to log
 * @param time The time of the message
 */
void Logger::_writeLogInFile(Logger::LogLevel level, const char *msg, std::string time)
{
	// Create directory if not exist
	if (mkdir("logs", 0777) == -1 && errno != EEXIST)
	{
		// TODO: Handle error
		std::cerr << "Error: " << std::strerror(errno) << std::endl;
		return;
	}
	int file = open(("logs/" + Logger::getLogFileName()).c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
	// TODO: Handle file error
	std::string log = Logger::_formater(level, msg, time, false);
	write(file, log.c_str(), log.size());
	write(file, "\n", 1);
	close(file);
}

/*  */
/* ---------------------------------- MAIN ---------------------------------- */
/*  */

/*
 * @brief Log a message
 *
 * @param level The level of the message
 * @param msg The message to log
 * @param ... The arguments to replace in the message
 */
void Logger::log(Logger::LogLevel level, const char *msg, ...)
{
	// Check if the logger is on
	if (Logger::getLogState() == false || (level == Logger::DEBUG && Logger::getLogDebugState() == false))
		return;

	// Create a buffer
	const int initialBufferSize = 1024;
	std::vector<char> buffer(initialBufferSize);

	va_list args;
	va_start(args, msg);
	int size = vsnprintf(buffer.data(), buffer.size(), msg, args);
	va_end(args);

	// Handling error
	if (size < 0)
	{
		// TODO: Handle error
		return;
	}

	// Resize the buffer if needed
	while (size >= static_cast<int>(buffer.size()))
	{
		buffer.resize(buffer.size() * 2);
		va_start(args, msg);
		size = vsnprintf(buffer.data(), buffer.size(), msg, args);
		va_end(args);
	}

	// Get the current time
	std::time_t t = std::time(NULL);
	char timeBuffer[80];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

	// Print the log

	Logger::_printLog(level, buffer.data(), timeBuffer);
	if (Logger::getLogFileState() == true)
		Logger::_writeLogInFile(level, buffer.data(), timeBuffer);
	
	// throw if level is FATAL
	if (level == Logger::FATAL)
		throw std::runtime_error(buffer.data());
}

/*  */
/* --------------------------------- SETTERS -------------------------------- */
/*  */

/*
 * @brief Set the state of the logger
 *
 * @param state The state of the logger
 */
void Logger::setLogState(bool state)
{
	Logger::_logState = state;
}

/*
 * @brief Set the state of the file logger
 *
 * @param state The state of the file logger
 */
void Logger::setLogFileState(bool state)
{
	Logger::_logFileState = state;
}

/*
 * @brief Set the state of the debug logger
 *
 * @param state The state of the debug logger
 */
void Logger::setLogDebugState(bool state)
{
	Logger::_logDebugState = state;
}

/*  */
/* --------------------------------- GETTERS -------------------------------- */
/*  */

/*
 * @brief Get the current state of the logger
 *
 * @return bool : The current state of the logger
 */
bool Logger::getLogState(void)
{
	return (Logger::_logState);
}

/*
 * @brief Get the current state of the file logger
 *
 * @return bool : The current state of the file logger
 */
bool Logger::getLogFileState(void)
{
	return (Logger::_logFileState);
}

/*
 * @brief Get the current state of the debug logger
 *
 * @return bool : The current state of the debug logger
 */
bool Logger::getLogDebugState(void)
{
	return (Logger::_logDebugState);
}

/*
 * @brief Get the current file name
 *
 * @return std::string : The current file name
 */
std::string Logger::getLogFileName(void)
{
	return (Logger::_logFileName);
}

/*
 * @brief Get the string representation of a log level
 *
 * @param level The log level
 *
 * @return std::string : The string representation of the log level
 */
std::string Logger::getLogLevelStr(Logger::LogLevel level)
{
	return (Logger::_logLevelStr[level]);
}

/*
 * @brief Get the color representation of a log level
 *
 * @param level The log level
 *
 * @return std::string : The color representation of the log level
 */
std::string Logger::getLogLevelColor(Logger::LogLevel level)
{
	return (Logger::_logLevelColor[level]);
}

/*
 * @brief Cleanup the logger
 */
// void Logger::cleanup(void)
// {
// 	_logLevelStr.clear();
// 	_logLevelColor.clear();
// }
