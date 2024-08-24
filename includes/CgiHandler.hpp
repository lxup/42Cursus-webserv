#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <map>

# include "Request.hpp"

class CgiHandler
{
	private:
		std::map<std::string, std::string>	_env;
		pid_t								_pid;
		int									_pipe[2];
		int									_exitStatus;
		std::string							_execPath;
	public:
		CgiHandler(void);
		~CgiHandler(void);

		/* GETTERS */
		std::map<std::string, std::string> getEnv(void) const { return _env; }
		pid_t getPid(void) const { return _pid; }
		int getPipe(int index) const { return _pipe[index]; }
		int getExitStatus(void) const { return _exitStatus; }

		/* SETTERS */
		// void setEnv(std::string key, std::string value) { _env[key] = value; }
		// void setPid(pid_t pid) { _pid = pid; }
		// void setPipe(int index, int value) { _pipe[index] = value; }
		// void setExitStatus(int exitStatus) { _exitStatus = exitStatus; }
		
		/* UTILS */
		int		init(Request* request);
		int		execute(void);
		void	reset(void);
};

#endif // CGIHANDLER_HPP