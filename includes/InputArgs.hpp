/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputArgs.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lquehec <lquehec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 22:50:42 by lquehec           #+#    #+#             */
/*   Updated: 2024/07/26 02:36:05 by lquehec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTARGS_HPP
# define INPUTARGS_HPP

# include "Webserv.hpp"

/* DEFAULT PARAMETERS */
# define DEFAULT_LOG_STATE true
# define DEFAULT_LOG_FILE_STATE false
# define DEFAULT_LOG_DEBUG_STATE false

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
};

#endif // INPUTARGS_HPP
