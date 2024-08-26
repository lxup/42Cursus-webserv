#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <map>
# include <sys/types.h>
# include <sys/wait.h>

# include "Request.hpp"

class Request;

class CgiHandler
{
	private:
		Request*							_request;
		std::map<std::string, std::string>	_env;
		std::string							_body;
		std::string							_bodyWithHeaders;
		pid_t								_pid;
		char								**_envp;
		char								**_argv;
		int									_StdinBackup;
		int									_StdoutBackup;
		FILE								*_tmpIn;
		FILE								*_tmpOut;
		long								_fdIn;
		long								_fdOut;

		// Utils
		char	**_envToChar(void);
		char	**_buildArgv(void);

		/* HANDLE */
		void	_handleExitStatus(void);
		
	public:
		CgiHandler(Request* request);
		~CgiHandler(void);

		/* GETTERS */
		Request* getRequest(void) const { return _request; }
		std::map<std::string, std::string> getEnv(void) const { return _env; }
		std::string getBody(void) const { return _body; }
		std::string getBodyWithHeaders(void) const { return _bodyWithHeaders; }
		// std::map<std::string, std::string> getEnv(void) const { return _env; }
		// pid_t getPid(void) const { return _pid; }
		// int getPipe(int index) const { return _pipe[index]; }
		// int getExitStatus(void) const { return _exitStatus; }

		/* SETTERS */
		// void setEnv(std::string key, std::string value) { _env[key] = value; }
		// void setPid(pid_t pid) { _pid = pid; }
		// void setPipe(int index, int value) { _pipe[index] = value; }
		// void setExitStatus(int exitStatus) { _exitStatus = exitStatus; }
		
		/* MAIN */
		void	init(void);
		void	execute(void);
		void	setHeaders(void);
};

#endif // CGIHANDLER_HPP