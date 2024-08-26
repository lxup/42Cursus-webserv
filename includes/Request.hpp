#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sstream>
# include <algorithm>

# include "Logger.hpp"
# include "Client.hpp"

# define REQUEST_DEFAULT_STATE_CODE 200
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

class Cgi
{
	private:
		bool		_isCGI;
		std::string	_path;
		std::string	_execPath;
	public:
		Cgi(void) : _isCGI(false), _path(""), _execPath("") {}
		~Cgi(void) {}

		// GETTERS
		bool		isCGI(void) const { return _isCGI; }
		std::string	getPath(void) const { return _path; }
		std::string	getExecPath(void) const { return _execPath; }

		// SETTERS
		void	setIsCGI(bool isCGI) { _isCGI = isCGI; }
		void	setPath(const std::string &path) { _path = path; }
		void	setExecPath(const std::string &execPath) { _execPath = execPath; }
};

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
		// std::map<std::string, std::string>	_query;
		std::string							_query;
		std::string							_httpVersion;
		std::string							_body;
		size_t								_bodySize;
		std::map<std::string, std::string>	_headers;
		// std::map<std::string, std::string>	_envCGI;
		// Chunked
		bool								_isChunked;
		int									_contentLength;
		int									_chunkSize;
		Cgi									_cgi;
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
		int		_findServer(void);
		int		_findLocation(void);
		// std::string	_findFilename(void);

		/* CHECKERS */
		int		_checkTransferEncoding(void);
		int 	_checkClientMaxBodySize(void);
		int		_checkMethod(void);
		int		_checkPathsMatch(const std::string &path, const std::string &parentPath);
		// int		_checkCGI(void);
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
		// Request(void);
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
		std::string getPath(void) const { return _path; }
		std::string getQuery(void) const { return _query; }
		std::string getHttpVersion(void) const { return _httpVersion; }
		std::string getBody(void) const { return _body; }
		size_t getBodySize(void) const { return _bodySize; }
		int getStatusCode(void) const { return _stateCode; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }
		bool isChunked(void) const { return _isChunked; }
		int getState(void) const { return _state; }
		int getContentLength(void) const { return _contentLength; }
		int getChunkSize(void) const { return _chunkSize; }
		// Cgi& getCgi(void) { return _cgi; }
		// CGI
		bool isCGI(void) const { return _cgi.isCGI(); }
		std::string getCgiPath(void) const { return _cgi.getPath(); }
		std::string getCgiExecPath(void) const { return _cgi.getExecPath(); }

		/* SETTERS */
		void	setError(int code);
		void 	setStateCode(int code) { _stateCode = code; }
		// CGI
		void	setCgi(bool isCgi, const std::string &path, const std::string &execPath) { _cgi.setIsCGI(isCgi); _cgi.setPath(path); _cgi.setExecPath(execPath); }
		// void	setIsCGI(bool isCGI) { _cgi.setIsCGI(isCGI); }
		// void	setPath(const std::string &path) { _cgi.setPath(path); }
		// void	setExecPath(const std::string &execPath) { _cgi.setExecPath(execPath); }
};

#endif // REQUEST_HPP