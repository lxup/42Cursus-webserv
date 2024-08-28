#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <map>
# include <sys/types.h>
# include <sys/wait.h>
# include <ctime>

# include "Utils.hpp"

class Response;

class CgiHandler
{
	friend class Response;
	public:
		enum e_cgi_handler_state
		{
			INIT,
			HEADERS_PARSE_KEY,
			HEADERS_PARSE_VALUE,
			HEADERS_PARSE_END,
			HEADERS_END,
			BODY,
			FINISH
		};
		static std::string	getStateStr(e_cgi_handler_state state);
	private:
		Response*							_response;
		std::string							_output;
		std::map<std::string, std::string>	_headers;
		std::string							_tmpHeaderKey;
		std::string							_tmpHeaderValue;
		bool								_isChunked;
		e_cgi_handler_state					_state;

		/* PARSING */
		void	_setState(e_cgi_handler_state state);
		void	_parse(const std::string &data);
		void	_parseHeaders(void);
		void	_parseHeadersKey(void);
		void	_parseHeadersValue(void);
		void	_parseBody(void);
		void	_parseChunkedBody(void);

		/* CHECKER */
		int		_checkHeaders(void);
	public:
		CgiHandler(Response* response);
		CgiHandler(const CgiHandler &src);
		~CgiHandler(void);

		CgiHandler &operator=(const CgiHandler &src);

		/* GETTERS */
		std::string getOutput(void) const { return _output; }

		e_cgi_handler_state getState(void) const { return _state; }
};

# include "Response.hpp"

#endif // CGIHANDLER_HPP