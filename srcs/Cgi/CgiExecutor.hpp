#pragma once
#ifndef CGIEXECUTOR_HPP
# define CGIEXECUTOR_HPP

# include <iostream>
# include <map>

# include "RequestCgi.hpp"
# include "RequestBody.hpp"

class RequestCgi;

class CgiExecutor
{
	friend class RequestCgi;
	public:
	private:
		RequestCgi*	_requestCgi;
		std::map<std::string, std::string>	_env;
		char**								_envp;
		char**								_argv; // 0: path, 1: execPath, 2: NULL
		// Headers
		std::map<std::string, std::string>	_headers;
		std::string							_tmpHeaderKey;
		std::string							_tmpHeaderValue;
		// Output
		RequestBody							_body;
		// FILE*								_tmpOut;
		// long								_fdOut;
		
		// Execution
		pid_t								_pid;
		int									_StdinBackup;
		int									_StdoutBackup;
		// State
		time_t								_lastActivity;

		/* METHODS */
		void	_init(void);
		void	_execute(void);

		/* UTILS */
		char**	_envToChar(void);
		char**	_buildArgv(void);
	public:
		CgiExecutor(RequestCgi* requestCgi);
		CgiExecutor(const CgiExecutor &src);
		~CgiExecutor(void);

		CgiExecutor &operator=(const CgiExecutor &src);

		/* GETTERS */
		int	getFdOut(void) const { return this->_body._fd; }
};

#endif // CGIEXECUTOR_HPP