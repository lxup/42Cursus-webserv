/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 12:28:49 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/25 14:57:48 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "Webserv.hpp"

class Logger
{
	public:
		/* ENUMs */
		enum LogLevel
		{
			FATAL = 0,
			ERROR,
			ALERT,
			WARNING,
			INFO,
			DEBUG,
			TRACE
		};

		/* MAIN */
		static void			log(LogLevel level, const char *msg, ...);

		/* SETTERS */
		static void			setLogState(bool state);
		static void			setLogFileState(bool state);

		/* GETTERS */
		static bool			getLogState(void);
		static bool			getLogFileState(void);
		static std::string	getLogFileName(void);
		static std::string	getLogLevelStr(LogLevel level);
		static std::string	getLogLevelColor(LogLevel level);

	private:
		static bool										_logState;
		static bool										_logFileState;
		static std::string								_logFileName;
		static std::map<Logger::LogLevel, std::string>	_logLevelStr;
		static std::map<Logger::LogLevel, std::string>	_logLevelColor;

		/* UTILS */
		static std::string								_generateLogFileName(void);
		static std::map<Logger::LogLevel, std::string>	_generateLogLevelStr(void);
		static std::map<Logger::LogLevel, std::string>	_generateLogLevelColor(void);
		static std::string								_formater(Logger::LogLevel level, const char *msg, std::string time, bool colored = true);
		static void										_printLog(Logger::LogLevel level, const char *msg, std::string time);
		static void										_writeLogInFile(LogLevel level, const char *msg, std::string time);
};

#endif // LOGGER_HPP