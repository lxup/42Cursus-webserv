#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"

class Request
{
	public:
		enum e_parse_state
		{
			INIT,
			HEADERS,
			BODY,
			FINISH,
			ERROR
		};
	private:
		std::string 						_rawRequest;
		std::string 						_method;
		std::string 						_uri;
		std::string							_httpVersion;
		std::string							_body;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		bool								_isChunked;
		e_parse_state						_state;
		int									_errorCode;

		void	_parseRequestLine(void);
		void	_parseHeaders(void);
		void	_parseBody(void);
		void	_parseChunkedBody(void);

		void	_setState(e_parse_state state);
		void	_setHeaderState(void);
		void	_setError(int code);
	public:
		Request(void);
		Request(const Request &src);
		~Request(void);

		Request &operator=(const Request &rhs);

		void	parse(const std::string &rawRequest);

		/* GETTERS */
		std::string	getRawRequest(void) const { return _rawRequest; }
		std::string getMethod(void) const { return _method; }
		std::string getUri(void) const { return _uri; }
		std::string getHttpVersion(void) const { return _httpVersion; }
		std::string getBody(void) const { return _body; }
		std::string getVersion(void) const { return _version; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool isChunked(void) const { return _isChunked; }
		int getState(void) const { return _state; }
		// bool isReady(void) const { return _isReady; }
};

#endif // REQUEST_HPP