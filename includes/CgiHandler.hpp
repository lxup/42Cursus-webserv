#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <map>
# include <sys/types.h>
# include <sys/wait.h>
# include <ctime>

# include "Request.hpp"
# include "Response.hpp"

class Request;
class Response;

# define CGI_READ_BUFFER_SIZE 4096

class CgiHandler
{
	public:
		enum e_cgi_state
		{
			INIT,
			PROCESS,
			FINISH
		};
		enum e_cgi_parse_state
		{
			P_INIT,
			P_HEADERS_PARSE_KEY,
			P_HEADERS_PARSE_VALUE,
			P_HEADERS_PARSE_END,
			P_HEADERS_END,
			P_BODY
		};
	private:
		Response*							_response;
		Request*							_request;
		std::map<std::string, std::string>	_env;
		std::string							_output;
		std::map<std::string, std::string>	_headers;
		std::string							_tmpHeaderKey;
		std::string							_tmpHeaderValue;
		std::string							_body;
		uint64_t							_contentLength;
		pid_t								_pid;
		char								**_envp;
		char								**_argv;
		int									_StdinBackup;
		int									_StdoutBackup;
		FILE								*_tmpIn;
		FILE								*_tmpOut;
		long								_fdIn;
		long								_fdOut;
		e_cgi_state							_state;
		e_cgi_parse_state					_parseState;
		time_t								_lastActivity;


		// Utils
		char	**_envToChar(void);
		char	**_buildArgv(void);

		/* PARSING */
		void	_setParseState(e_cgi_parse_state state);
		void	_parse(const std::string &data);
		void	_parseHeaders(void);
		void	_parseHeadersKey(void);
		void	_parseHeadersValue(void);
		void	_parseBody(void);

		/* CHECKER */
		void	_checkHeaders(void);

		
	public:
		CgiHandler(Response* response, Request* request);
		~CgiHandler(void);

		/* GETTERS */
		Request* getRequest(void) const { return _request; }
		std::map<std::string, std::string> getEnv(void) const { return _env; }
		std::string getBody(void) const { return _body; }
		std::string getOutput(void) const { return _output; }

		e_cgi_state getState(void) const { return _state; }
		e_cgi_parse_state getParseState(void) const { return _parseState; }
		long	getFdIn(void) const { return _fdIn; }
		long	getFdOut(void) const { return _fdOut; }
		int getPid(void) const { return _pid; }

		/* CHECKER */
		void	checkState(void);


		/* TIMEOUT */
		time_t getLastActivity() const { return _lastActivity; }
		void updateLastActivity() { _lastActivity = time(NULL); }


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
		std::string	buildResponse(void);
};

#endif // CGIHANDLER_HPP