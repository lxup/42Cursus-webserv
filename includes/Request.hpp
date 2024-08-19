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
			START,
			HEADERS,
			BODY,
			DONE
		};
	private:
		std::string							_rawRequest;
		std::string 						_method;
		std::string 						_uri;
		std::string							_httpVersion;
		std::string							_body;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		bool								_isChunked;
		bool								_isReady;

		/* SETTERS */
		void	_setRawRequest(const std::string &rawRequest) { _rawRequest = rawRequest; }

		void	_parse(void);
		void	_parseRequestLine(std::istringstream &iss, std::string &line, int &step);
		void	_parseHeaders(std::istringstream &iss, std::string &line, int &step);
		void	_parseBody(std::istringstream &iss, std::string &line, int &step);
		void	_parseChunkedBody(std::istringstream &iss, std::string &line, int &step);

		void	_setHeaderState(void);
	public:
		Request(void);
		Request(const std::string &rawRequest);
		Request(const Request &src);
		~Request(void);

		Request &operator=(const Request &rhs);

		/* GETTERS */
		std::string getMethod(void) const { return _method; }
		std::string getUri(void) const { return _uri; }
		std::string getHttpVersion(void) const { return _httpVersion; }
		std::string getBody(void) const { return _body; }
		std::string getVersion(void) const { return _version; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool isReady(void) const { return _isReady; }
};

#endif // REQUEST_HPP