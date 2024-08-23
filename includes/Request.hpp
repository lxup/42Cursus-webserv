#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"
# include "Client.hpp"

# define REQUEST_DEFAULT_STATEC_ODE 200
# define REQUEST_DEFAULT_UPLOAD_PATH "./www/upload/"

class Client;

/*class File
{
	private:
		std::ofstream				_file;
		std::string					_filename;
		std::string					_path;
	public:
		File(void) : _filename(""), _path("") {}
		File(const std::string &filename) : _filename(filename), _path("") {}
		~File(void) { if (_file.is_open()) _file.close(); }

		// GETTERS
		std::ofstream*	getFile(void) { return &_file; }
		std::string		getFilename(void) const { return _filename; }
		std::string		getPath(void) const { return _path; }

		// SETTERS
		void	setFilename(const std::string &filename) { _filename = filename; }
		void	setPath(const std::string &path) { _path = path; }
};*/

class Request
{
	public:
		enum	e_parse_state
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
		std::map<std::string, std::string>	_query;
		std::string							_httpVersion;
		std::string							_body;
		size_t								_bodySize;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_envCGI;
		// Chunked
		bool								_isChunked;
		int									_contentLength;
		int									_chunkSize;
		// File								_file;
		e_parse_state						_state;
		int									_stateCode;

		void	_parseRequestLine(void);
		void	_parseHeaders(void);
		void	_parseBody(void);
		void	_parseChunkedBody(void);

		void	_setState(e_parse_state state);
		void	_setHeaderState(void);

		/* PROCESS */
		void	_processUri(void);

		/* FINDERS */
		int			_findServer(void);
		int			_findLocation(void);
		// std::string	_findFilename(void);

		/* CHECKERS */
		int		_checkTransferEncoding(void);
		int 	_checkClientMaxBodySize(void);
		int		_checkMethod(void);
		int		_checkPathsMatch(const std::string &path, const std::string &parentPath);
		// int		_checkContentType(void);

		/* HANDLE */
		// int		_handleMultipartFormData(void);
		// int		_handleOctetStream(void);

		/* TOOLS */
		// std::string	_extractContentType(const std::string &contentType);
		// std::string	_getRandomFilename(void);
		// void		_uploadFailed(void);

		/* INIT */
		void	_initServer(void);
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

		/* SETTERS */
		void	setError(int code);
};

#endif // REQUEST_HPP