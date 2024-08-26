#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"
# include "Client.hpp"
# include "ConfigParser.hpp"

# define REQUEST_DEFAULT_STATE_CODE 200
# define REQUEST_DEFAULT_UPLOAD_PATH "./www/upload/"

class Client;
class Request
{
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
			BODY,
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
		std::string							_body;
		size_t								_bodySize;
		std::map<std::string, std::string>	_headers;
		std::string							_tmpHeaderKey;
		std::string							_tmpHeaderValue;
		bool								_isChunked;
		int									_contentLength;
		int									_chunkSize;
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
		int		_checkPathsMatch(const std::string &path, const std::string &parentPath);

		/* INIT */
		void	_initServer(void);
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
		std::string 	getBody(void) const { return _body; }
		size_t 			getBodySize(void) const { return _bodySize; }
		int 			getStateCode(void) const { return _stateCode; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool 			isChunked(void) const { return _isChunked; }
		int 			getState(void) const { return _state; }
		int 			getContentLength(void) const { return _contentLength; }
		int 			getChunkSize(void) const { return _chunkSize; }
		/* SETTERS */
		void			setError(int code);
		void 			setStateCode(int code) { _stateCode = code; }
};

#endif // REQUEST_HPP