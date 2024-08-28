#ifndef REQUESTCGI_HPP
# define REQUESTCGI_HPP

# include <iostream>

// # include "CgiHandler.hpp"
# include "Utils.hpp"
# include "CgiExecutor.hpp"
// # include "Request.hpp"

class Request;
class CgiExecutor;
// class CgiHandler;

class RequestCgi
{
	friend class Client;
	friend class Request;
	friend class Response;
	friend class CgiExecutor;
	private:
		Request*		_request;
		bool			_isCGI;
		std::string		_path;
		std::string		_execPath;
		CgiExecutor*	_cgiHandler;

		/* METHODS */
		void	_start(void);
		void	_checkState(void);
		void	_kill(void);
	public:
		RequestCgi(void);
		RequestCgi(Request* request);
		RequestCgi(const RequestCgi &src);
		~RequestCgi(void);

		RequestCgi &operator=(const RequestCgi &src);
};

# include "Request.hpp"

#endif // REQUESTCGI_HPP