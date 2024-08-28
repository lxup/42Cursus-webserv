#pragma once
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"
# include "Client.hpp"
# include "ConfigParser.hpp"
# include "RequestCgi.hpp"
# include "RequestBody.hpp"

# define REQUEST_DEFAULT_STATE_CODE 200
# define REQUEST_DEFAULT_UPLOAD_PATH "./www/upload/"
# define REQUEST_DEFAULT_HEADER_TIMEOUT 1
# define REQUEST_DEFAULT_BODY_TIMEOUT 1

class Client;
// class RequestCgi;
class RequestBody;

class Request
{
	friend class RequestCgi;
	friend class RequestBody;
	friend class CgiHandler;
	friend class CgiHandlerV2;
	public:
		enum	e_parse_state
		{
			INIT,
			REQUEST_LINE_METHOD,
			REQUEST_LINE_URI,
			REQUEST_LINE_HTTP_VERSION,
			REQUEST_LINE_END,
			HEADERS_INIT,
			HEADERS_PARSE_KEY,
			HEADERS_PARSE_VALUE,
			HEADERS_PARSE_END,
			HEADERS_END,
			BODY_INIT,
			BODY_PROCESS,
			BODY_END,
			CGI_INIT,
			CGI_PROCESS,
			CGI_END,
			FINISH
		};
		std::string	getParseStateStr(e_parse_state state) const;
	private:
		Client*								_client;
		BlocServer*							_server;
		BlocLocation*						_location;
		std::string 						_rawRequest;
		std::string 						_method;
		std::string 						_uri;
		std::string 						_path;
		std::string							_query;
		std::string							_httpVersion;
		RequestBody							_body;
		// std::string							_body;
		// u_int64_t							_bodySize;
		std::map<std::string, std::string>	_headers;
		std::string							_tmpHeaderKey;
		std::string							_tmpHeaderValue;
		bool								_isChunked;
		RequestCgi							_cgi;
		int									_contentLength;
		int									_chunkSize;
		time_t								_timeout;
		e_parse_state						_state;
		int									_stateCode;

		/* PARSERS */
		// Request line
		void	_parseRequestLine(void);
		void	_parseMethod(void);
		void	_parseUri(void);
		void	_parseHttpVersion(void);
		// Headers
		void	_parseHeaders(void);
		void	_parseHeadersKey(void);
		void	_parseHeadersValue(void);
		// Body
		void	_parseBody(void);
		void	_parseChunkedBody(void);

		void	_setState(e_parse_state state);
		void	_setHeaderState(void);

		/* PROCESS */
		void	_processUri(void);

		/* FINDERS */
		int		_findServer(void);
		int		_findLocation(void);

		/* CHECKERS */
		int		_checkTransferEncoding(void);
		int 	_checkClientMaxBodySize(void);
		int		_checkMethod(void);
		int		_checkCgi(void);
		int		_checkPathsMatch(const std::string &path, const std::string &parentPath);

		/* BODY */
		void	_defineBodyDestination(void);

		/* TIMEOUT */
		void	_initTimeout(void);

		/* INIT */
		void	_initServer(void);

		/* TOOLS */
		std::vector<std::string> _getAllPathsLocation(void);
	public:
		Request(Client *client);
		Request(const Request &src);
		~Request(void);

		Request &operator=(const Request &rhs);

		void	parse(const std::string &rawRequest);

		/* GETTERS */
		Client*			getClient(void) const { return _client; }
		BlocServer*		getServer(void) const { return _server; }
		BlocLocation*	getLocation(void) const { return _location; }
		std::string		getRawRequest(void) const { return _rawRequest; }
		std::string 	getMethod(void) const { return _method; }
		std::string 	getUri(void) const { return _uri; }
		std::string 	getPath(void) const { return _path; }
		std::string 	getQuery(void) const { return _query; }
		std::string 	getHttpVersion(void) const { return _httpVersion; }
		// std::string 	getBody(void) const { return _body; }
		RequestBody		getBody(void) const { return _body; }
		// size_t 			getBodySize(void) const { return _bodySize; }
		int 			getStateCode(void) const { return _stateCode; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool 			isChunked(void) const { return _isChunked; }
		int 			getState(void) const { return _state; }
		int 			getContentLength(void) const { return _contentLength; }
		int 			getChunkSize(void) const { return _chunkSize; }
		time_t			getTimeout(void) const { return _timeout; }
		/* SETTERS */
		void			setError(int code);
		void 			setStateCode(int code) { _stateCode = code; }
		void			setTimeout(time_t timeout) { _timeout = timeout; }
		void			setTimeout(int timeout) { _timeout = time(NULL) + timeout; }
		void			setCgi(bool isCgi, const std::string &path, const std::string &execPath) { _cgi._isCGI = isCgi; _cgi._path = path; _cgi._execPath = execPath; }

		/* TIMEOUT */
		void			checkTimeout(int epollfd);
};

#endif // REQUEST_HPP