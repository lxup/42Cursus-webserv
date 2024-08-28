#ifndef REQUESTCGI_HPP
# define REQUESTCGI_HPP

# include <iostream>

// # include "CgiHandler.hpp"
# include "Utils.hpp"
# include "CgiHandlerV2.hpp"
// # include "Request.hpp"

class Request;
class CgiHandlerV2;
// class CgiHandler;

class RequestCgi
{
	friend class Request;
	private:
		Request*		_request;
		bool			_isCGI;
		std::string		_path;
		std::string		_execPath;
		CgiHandlerV2*	_cgiHandler;

		/* METHODS */
		void	_start(void);
	public:
		RequestCgi(void);
		RequestCgi(Request* request);
		RequestCgi(const RequestCgi &src);
		~RequestCgi(void);

		RequestCgi &operator=(const RequestCgi &src);
};

# include "Request.hpp"

#endif // REQUESTCGI_HPP