/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 11:50:11 by lquehec           #+#    #+#             */
/*   Updated: 2024/08/07 12:25:39 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Logger.hpp"
#include <vector>
#include <cstdio>
#include <cstring>

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

#endif // UTILS_HPP
