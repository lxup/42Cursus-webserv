/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputArgs.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 22:50:42 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/26 02:57:38 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTARGS_HPP
# define INPUTARGS_HPP

# include "Webserv.hpp"

/* DEFAULT PARAMETERS */
# define DEFAULT_CONFIG_FILE_PATH	"./config/default.conf"

class InputArgs
{
	private:
		int							_ac;
		char**						_av;
		std::string					_configFilePath;
		std::map<std::string, bool>	_options;
		int							_state;

		/* UTILS */
		void						_parse(void);
		std::map<std::string, bool>	_generateOptions(void);
		std::string					_convertToLongOption(std::string option);
		std::string					_convertToShortOption(std::string option);
	public:
		InputArgs(int argc, char **argv);
		~InputArgs(void);

		bool						isOption(std::string option);
		void						help(void);
		void						summary(void);

		/* GETTERS */
		int							getState(void) const;

		/* EXCEPTIONS */
		// class InvalidOption : public std::exception
		// {
		// 	private:
		// 		std::string _message;
		// 	public:
		// 		InvalidOption(void) {}
		// 		InvalidOption(std::string option) : _message(option.empty() ? "illegal option" : "illegal option -- " + option) {}
		// 		virtual ~InvalidOption(void) throw() {}
		// 		virtual const char* what() const throw();
		// };

		// class InvalidArgument : public std::exception
		// {
		// 	private:
		// 		std::string _message;
		// 	public:
		// 		InvalidArgument(void) {}
		// 		InvalidArgument(std::string argument) : _message(argument.empty() ? "invalid argument" : "invalid argument -- " + argument) {}
		// 		virtual ~InvalidArgument(void) throw() {}
		// 		virtual const char* what() const throw();
		// };
};

#endif // INPUTARGS_HPP
