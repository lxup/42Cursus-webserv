#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"
# include "Client.hpp"

# define REQUEST_DEFAULT_STATEC_ODE 200

class Client;

class Request
{
	public:
		enum e_parse_state
		{
			INIT,
			HEADERS,
			BODY,
			FINISH
		};
	private:
		Client*								_client;
		BlocServer*							_server;
		BlocLocation*						_location;
		std::string 						_rawRequest;
		std::string 						_method;
		std::string 						_uri;
		std::string 						_path;
		std::string 						_query;
		std::string							_httpVersion;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_envCGI;
		bool								_isChunked;
		int									_contentLength;
		int									_chunkSize;
		e_parse_state						_state;
		int									_stateCode;

		void	_parseRequestLine(void);
		void	_parseHeaders(void);
		void	_parseBody(void);
		void	_parseChunkedBody(void);

		void	_setState(e_parse_state state);
		void	_setHeaderState(void);
		void	_setError(int code);

		/* PROCESS */
		void	_processUri(void);

		/* FINDERS */
		int		_findServer(void);
		int		_findLocation(void);

		/* CHECKERS */
		int 	_checkClientMaxBodySize(void);
		int		_checkMethod(void);
		int		_checkPathsMatch(const std::string &path, const std::string &parentPath);
	public:
		Request(void);
		Request(Client *client);
		Request(const Request &src);
		~Request(void);

		Request &operator=(const Request &rhs);

		void	parse(const std::string &rawRequest);

		/* GETTERS */
		Client*		getClient(void) const { return _client; }
		BlocServer*	getServer(void) const { return _server; }
		BlocLocation* getLocation(void) const { return _location; }
		std::string	getRawRequest(void) const { return _rawRequest; }
		std::string getMethod(void) const { return _method; }
		std::string getUri(void) const { return _uri; }
		std::string getHttpVersion(void) const { return _httpVersion; }
		std::string getBody(void) const { return _body; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool isChunked(void) const { return _isChunked; }
		int getState(void) const { return _state; }
		// bool isReady(void) const { return _isReady; }
};

#endif // REQUEST_HPP