#include "Request.hpp"
#include "Webserv.hpp"

// Request::Request(void) : _client(NULL), _server(NULL), _location(NULL), _rawRequest(""), _method(""), _uri(""), _path(""), _httpVersion(""), _body(""), _bodySize(0), _isChunked(false), _contentLength(-1), _chunkSize(-1), _state(Request::INIT), _stateCode(REQUEST_DEFAULT_STATE_CODE)
// {

// }

std::string	Request::getParseStateStr(e_parse_state state)
{
	switch (state)
	{
		case INIT:
			return "INIT";
		case REQUEST_LINE_METHOD:
			return "REQUEST_LINE_METHOD";
		case REQUEST_LINE_URI:
			return "REQUEST_LINE_URI";
		case REQUEST_LINE_HTTP_VERSION:
			return "REQUEST_LINE_HTTP_VERSION";
		case REQUEST_LINE_END:
			return "REQUEST_LINE_END";
		case HEADERS_INIT:
			return "HEADERS_INIT";
		case HEADERS_PARSE_KEY:
			return "HEADERS_PARSE_KEY";
		case HEADERS_PARSE_VALUE:
			return "HEADERS_PARSE_VALUE";
		case HEADERS_PARSE_END:
			return "HEADERS_PARSE_END";
		case HEADERS_END:
			return "HEADERS_END";
		case BODY_INIT:
			return "BODY_INIT";
		case BODY_PROCESS:
			return "BODY_PROCESS";
		case BODY_END:
			return "BODY_END";
		case CGI_INIT:
			return "CGI_INIT";
		case CGI_PROCESS:
			return "CGI_PROCESS";
		case CGI_END:
			return "CGI_END";
		case FINISH:
			return "FINISH";
		default:
			return "UNKNOWN";
	}
}

Request::Request(Client *client) : _client(client), _server(NULL), _location(NULL),  _rawRequest(""), _method(""), _uri(""), _path(""), _httpVersion(""), _isChunked(false), _cgi(this), _contentLength(-1),  _chunkSize(-1), _timeout(0), _state(Request::INIT), _stateCode(REQUEST_DEFAULT_STATE_CODE)
{
	this->_initServer();
}

Request::Request(const Request &src)
{
	*this = src;
}

Request::~Request(void)
{
	// if (this->_file.getFile()->is_open())
	// 	this->_file.getFile()->close();
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_client = rhs._client;
		this->_server = rhs._server;
		this->_location = rhs._location;
		this->_rawRequest = rhs._rawRequest;
		this->_method = rhs._method;
		this->_uri = rhs._uri;
		this->_path = rhs._path;
		this->_query = rhs._query;
		this->_httpVersion = rhs._httpVersion;
		this->_body = rhs._body;
		this->_headers = rhs._headers;
		// this->_envCGI = rhs._envCGI;
		this->_isChunked = rhs._isChunked;
		this->_contentLength = rhs._contentLength;
		this->_chunkSize = rhs._chunkSize;
		// this->_cgi = rhs._cgi;
		this->_state = rhs._state;
		this->_stateCode = rhs._stateCode;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** @brief Parse the raw request
*/
void	Request::parse(const std::string &rawRequest)
{
	if (this->_state == Request::FINISH)
		return ;
	if (this->_state == Request::INIT)
	{
		Logger::log(Logger::TRACE, "\"%.*s\"", 25, rawRequest.substr(0, rawRequest.find("\n")).c_str());
		this->_initTimeout();
	}
	

	if (rawRequest.empty())
	{
		Logger::log(Logger::WARNING, "Empty request");
		return ;
	}
	this->_rawRequest += rawRequest;

	// Logger::log(Logger::ERROR, "ACTUAL BODY SIZE: %zu", this->_bodySize);
	// Logger::log(Logger::ERROR, "RAW REQUEST SIZE: %zu", this->_rawRequest.length());
	// Logger::log(Logger::ERROR, "EXPECTED NEW BODY SIZE: %zu", this->_bodySize + this->_rawRequest.size());
	Logger::log(Logger::DEBUG, "Parsing request: %s", this->_rawRequest.c_str());

	this->_parseRequestLine();
	this->_parseHeaders();
	this->_parseBody();
}

/*
** @brief Parse the request line
**
** @param iss : The input stream
** @param step : The current step
*/
void	Request::_parseRequestLine(void)
{
	if (this->_state > Request::REQUEST_LINE_END)
		return (Logger::log(Logger::DEBUG, "Request line already parsed"));
	if (this->_state == Request::INIT)
		this->_setState(Request::REQUEST_LINE_METHOD);

	if (this->_state == Request::REQUEST_LINE_METHOD)
		this->_parseMethod();
	if (this->_state == Request::REQUEST_LINE_URI)
		this->_parseUri();
	if (this->_state == Request::REQUEST_LINE_HTTP_VERSION)
		this->_parseHttpVersion();
	if (this->_state == Request::REQUEST_LINE_END)
	{
		this->_rawRequest.erase(0, this->_rawRequest.find_first_not_of(" \t"));
		if (this->_rawRequest.empty())
			return ;
		if (this->_rawRequest[0] == '\n')
		{
			this->_rawRequest.erase(0, 1);
			return (this->_setState(Request::HEADERS_INIT));
		}
		if (this->_rawRequest[0] == '\r')
		{
			if (this->_rawRequest.size() < 2)
				return ;
			if (this->_rawRequest[1] == '\n')
			{
				this->_rawRequest.erase(0, 2);
				return (this->_setState(Request::HEADERS_INIT));
			}
			return (this->setError(400));
		}
		return (this->setError(400));
	}	
}

/*
** @brief Parse the method
**
*/
void	Request::_parseMethod(void)
{
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize)
	{
		if (this->_rawRequest[i] == ' ')
		{
			found = true;
			break;
		}
		if (!std::isalpha(this->_rawRequest[i]))
			return (this->setError(400));
		this->_method += this->_rawRequest[i];
		i++;
	}
	this->_rawRequest.erase(0, found ? i + 1 : i);
	if (found)
	{
		if (this->_method.empty())
			return (this->setError(400));
		if (ConfigParser::isMethodSupported(this->_method) == false)
			return (this->setError(405));
		Logger::log(Logger::DEBUG, "Method: %s", this->_method.c_str());
		this->_setState(Request::REQUEST_LINE_URI);
	}
}

/*
** @brief Parse the URI
**
*/
void	Request::_parseUri(void)
{
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize)
	{
		if (this->_uri.empty() && (this->_rawRequest[i] == ' ' || this->_rawRequest[i] == '\t'))
		{
			i++;
			continue;
		}
		if (this->_rawRequest[i] == ' ')
		{
			found = true;
			break;
		}
		if (!std::isprint(this->_rawRequest[i]))
			return (this->setError(400));
		this->_uri += this->_rawRequest[i];
		i++;
	}
	this->_rawRequest.erase(0, found ? i + 1 : i);
	if (found)
	{
		if (this->_uri.empty())
			return (this->setError(400));
		// this->_uri.erase(std::remove_if(this->_uri.begin(), this->_uri.end(), ::isspace), this->_uri.end());
		if (this->_processUri() == -1)
			return ;
		Logger::log(Logger::DEBUG, "URI: %s", this->_uri.c_str());
		return (this->_setState(Request::REQUEST_LINE_HTTP_VERSION));
	}
	// size_t pos = this->_rawRequest.find(" ");
	// if (pos == std::string::npos)
	// {
	// 	this->_uri += this->_rawRequest;
	// 	this->_rawRequest.clear();
	// 	return ;
	// }
	// this->_uri += this->_rawRequest.substr(0, pos);
	// this->_rawRequest.erase(0, pos + 1);

	// this->_uri.erase(std::remove_if(this->_uri.begin(), this->_uri.end(), ::isspace), this->_uri.end());

	// Logger::log(Logger::DEBUG, "URI: %s", this->_uri.c_str());
	// this->_setState(Request::REQUEST_LINE_HTTP_VERSION);
	// this->_processUri();
}

/*
** @brief Parse the HTTP version
**
*/
void	Request::_parseHttpVersion(void)
{
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize)
	{
		if (this->_httpVersion.empty() && (this->_rawRequest[i] == ' ' || this->_rawRequest[i] == '\t'))
		{
			i++;
			continue;
		}
		if (this->_rawRequest[i] != 'H' && this->_rawRequest[i] != 'T' && this->_rawRequest[i] != 'P' && this->_rawRequest[i] != '/' && this->_rawRequest[i] != '.' && !std::isdigit(this->_rawRequest[i]))
		{
			found = true;
			break;
		}
		this->_httpVersion += this->_rawRequest[i];
		i++;
	}
	this->_rawRequest.erase(0, i);
	// std::cout << C_YELLOW << "FOUND: " << found << C_RESET << std::endl;
	// std::cout << C_YELLOW << "RAW REQUEST: '" << this->_rawRequest << "'" << C_RESET << std::endl;
	// std::cout << C_YELLOW << "HTTP VERSION: '" << this->_httpVersion << "'" << C_RESET << std::endl;
	if (found)
	{
		if (this->_httpVersion.empty())
			return (this->setError(400));
		// this->_httpVersion.erase(std::remove_if(this->_httpVersion.begin(), this->_httpVersion.end(), ::isspace), this->_httpVersion.end());
		if (ConfigParser::isHttpVersionSupported(this->_httpVersion) == false)
			return (this->setError(505));
		this->_setState(Request::REQUEST_LINE_END);
	}
	// size_t pos = this->_rawRequest.find("\r\n");
	// if (pos == std::string::npos)
	// {
	// 	this->_httpVersion += this->_rawRequest;
	// 	this->_rawRequest.clear();
	// 	return ;
	// }
	// this->_httpVersion += this->_rawRequest.substr(0, pos);
	// this->_rawRequest.erase(0, pos + 2);

	// this->_httpVersion.erase(std::remove_if(this->_httpVersion.begin(), this->_httpVersion.end(), ::isspace), this->_httpVersion.end());

	// Logger::log(Logger::DEBUG, "HTTP Version: %s", this->_httpVersion.c_str());
	// if (Server::isHttpVersionSupported(this->_httpVersion) == false)
	// 	return (this->setError(505));
	// this->_setState(Request::HEADERS);
}


/*
** @brief Parse the headers
**
*/
void	Request::_parseHeaders(void)
{
	if (this->_state < Request::HEADERS_INIT)
		return (Logger::log(Logger::DEBUG, "Request line not parsed yet"));
	if (this->_state > Request::HEADERS_END)
		return (Logger::log(Logger::DEBUG, "Headers already parsed"));

	if (this->_state == Request::HEADERS_INIT)
		this->_setState(Request::HEADERS_PARSE_KEY);
	
	if (this->_state == Request::HEADERS_PARSE_KEY)
		this->_parseHeadersKey();
	if (this->_state == Request::HEADERS_PARSE_VALUE)
		this->_parseHeadersValue();
	if (this->_state == Request::HEADERS_PARSE_END)
	{
		this->_rawRequest.erase(0, this->_rawRequest.find_first_not_of(" \t"));
		if (this->_rawRequest.empty())
			return ;
		if (this->_rawRequest[0] == '\n')
		{
			this->_rawRequest.erase(0, 1);
			this->_setState(Request::HEADERS_PARSE_KEY);
			return (this->_parseHeaders());
		}
		if (this->_rawRequest[0] == '\r')
		{
			if (this->_rawRequest.size() < 2)
				return ;
			if (this->_rawRequest[1] == '\n')
			{
				this->_rawRequest.erase(0, 2);
				this->_setState(Request::HEADERS_PARSE_KEY);
				return (this->_parseHeaders());
			}
			return (this->setError(400));
		}
		return (this->setError(400));
	}
}

/*
** @brief Parse the headers key
**
*/
void	Request::_parseHeadersKey(void)
{
	// detect end of headers
	if (!this->_rawRequest.empty() && (this->_rawRequest[0] == '\r' || this->_rawRequest[0] == '\n'))
	{
		if (this->_rawRequest[0] == '\n')
		{
			this->_rawRequest.erase(0, 1);
			if (!this->_tmpHeaderKey.empty())
				return (this->setError(400));
			return (this->_setState(Request::BODY_INIT));
		}
		if (this->_rawRequest.size() < 2)
			return ;
		if (this->_rawRequest[1] == '\n')
		{
			this->_rawRequest.erase(0, 2);
			if (!this->_tmpHeaderKey.empty())
				return (this->setError(400));
			return (this->_setState(Request::BODY_INIT));
		}
		return (this->setError(400));
	}
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize)
	{
		if (this->_rawRequest[i] == ' ' || this->_rawRequest[i] == '\t')
			return (this->setError(400));
		if (this->_rawRequest[i] == ':')
		{
			found = true;
			break;
		}
		// if (!std::isprint(this->_rawRequest[i]))
		// 	return (this->setError(400));
		if (!std::isalnum(this->_rawRequest[i]) && this->_rawRequest[i] != '-' && this->_rawRequest[i] != '_')
			return (this->setError(400));
		this->_tmpHeaderKey += this->_rawRequest[i];
		i++;
	}
	this->_rawRequest.erase(0, found ? i + 1 : i);
	if (found)
	{
		if (this->_tmpHeaderKey.empty())
			return (this->setError(400));
		this->_tmpHeaderKey.erase(std::remove_if(this->_tmpHeaderKey.begin(), this->_tmpHeaderKey.end(), ::isspace), this->_tmpHeaderKey.end());
		Logger::log(Logger::DEBUG, "Header key: %s", this->_tmpHeaderKey.c_str());
		this->_setState(Request::HEADERS_PARSE_VALUE);
	}

}

/*
** @brief Parse the headers value
**
*/
void	Request::_parseHeadersValue(void)
{
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize)
	{
		// skip space and tab
		if (this->_tmpHeaderValue.empty() && (this->_rawRequest[i] == ' ' || this->_rawRequest[i] == '\t'))
		{
			i++;
			continue;
		}
		if (!std::isprint(this->_rawRequest[i]))
		{
			found = true;
			break;
		}
		this->_tmpHeaderValue += this->_rawRequest[i];
		i++;
	}
	this->_rawRequest.erase(0, i);
	if (found)
	{
		if (this->_tmpHeaderValue.empty())
			return (this->setError(400));
		this->_tmpHeaderValue.erase(std::remove_if(this->_tmpHeaderValue.begin(), this->_tmpHeaderValue.end(), ::isspace), this->_tmpHeaderValue.end());
		if (this->_headers.find(this->_tmpHeaderKey) != this->_headers.end())
			return (this->setError(400));
		Logger::log(Logger::DEBUG, "Header value: %s", this->_tmpHeaderValue.c_str());
		this->_headers[this->_tmpHeaderKey] = this->_tmpHeaderValue;
		this->_tmpHeaderKey.clear();
		this->_tmpHeaderValue.clear();
		this->_setState(Request::HEADERS_PARSE_END);
	}
}


/*
** @brief Parse the body
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void	Request::_parseBody(void)
{
	if (this->_state < Request::BODY_INIT)
		return (Logger::log(Logger::DEBUG, "Headers not parsed yet"));
	if (this->_state > Request::BODY_INIT)
		return (Logger::log(Logger::DEBUG, "Body already parsed"));

	if (this->isChunked())
		return (this->_parseChunkedBody());

	if (this->_body._write(this->_rawRequest) == -1)
		return (this->setError(500));
	this->_rawRequest.clear();
	if (this->_body._size > (u_int64_t)this->_server->getClientMaxBodySize()) // Check the client max body size
		return (this->setError(413));
	if (this->_body._size == (u_int64_t)this->_contentLength)
		return (this->_setState(Request::BODY_END));
	// if (this->_body._write(this->_rawRequest.substr(0, (u_int64_t)this->_contentLength - this->_body._size)) == -1)
	// 	return (this->setError(500));
	// this->_rawRequest.erase(0, (u_int64_t)this->_contentLength - this->_body._size);
	// if (this->_body._size == (u_int64_t)this->_contentLength)
	// 	return (this->_setState(Request::BODY_END));
}

/*
** @brief Parse the chunked body
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void Request::_parseChunkedBody(void)
{
	while (!this->_rawRequest.empty())
	{
		if (this->_chunkSize == -1)
		{
			size_t pos = this->_rawRequest.find("\r\n");
			if (pos == std::string::npos)
				return ; // Waiting for more data
			std::string line = this->_rawRequest.substr(0, pos);
			std::istringstream iss(line);
			if (!(iss >> std::hex >> this->_chunkSize))
			{
				this->setError(400);
				return (Logger::log(Logger::ERROR, "[_parseChunkedBody] Error parsing chunk size"));
			}
			this->_rawRequest.erase(0, pos + 2);
			if (this->_chunkSize == 0)
				return (this->_setState(Request::BODY_END));
			Logger::log(Logger::DEBUG, "[_parseChunkedBody] Chunk size: %d", this->_chunkSize);
		}
		size_t pos = this->_rawRequest.find("\r\n");
		if (pos == std::string::npos)
			return ; // Waiting for more data
		if (pos != (size_t)this->_chunkSize)
		{
			this->setError(400);
			return (Logger::log(Logger::ERROR, "[_parseChunkedBody] Chunk size does not match"));
		}
		if (this->_body._write(this->_rawRequest.substr(0, this->_chunkSize)) == -1)
			return (this->setError(500));
		this->_rawRequest.erase(0, this->_chunkSize + 2);
		this->_chunkSize = -1;
		if (this->_body._size > (u_int64_t)this->_server->getClientMaxBodySize()) // Check the client max body size
			return (this->setError(413));
	}
}

/*
** @brief Set the state
**
** @param state : The state
*/
void	Request::_setState(e_parse_state state)	
{
	if (this->_state == Request::FINISH)
		return (Logger::log(Logger::DEBUG, "[_setState] Request already finished"));
	if (this->_state == state)
		return (Logger::log(Logger::DEBUG, "[_setState] Request already in this state"));

	Logger::log(Logger::DEBUG, "[_setState] Request state changed from %s to %s with state code: %d", this->getParseStateStr(this->_state).c_str(), this->getParseStateStr(state).c_str(), this->_stateCode);
	this->_state = state;

	if (this->_state == Request::BODY_INIT)
	{
		this->_setHeaderState(); // Set the header state
		if (this->_state == Request::FINISH)
			return ;
		if (this->_method != "POST" && this->_method != "PUT")
			return(this->_cgi._isCGI ? this->_setState(Request::CGI_INIT) : this->_setState(Request::FINISH));
		this->setTimeout(REQUEST_DEFAULT_BODY_TIMEOUT);
		this->_defineBodyDestination();
	}
	else if (this->_state == Request::BODY_END)
	{
		if (this->_cgi._isCGI)
			this->_setState(Request::CGI_INIT);
		else
			this->_setState(Request::FINISH);
	}
	else if (this->_state == Request::CGI_INIT)
	{
		modifySocketEpoll(g_server.getEpollFD(), this->_client->getFd(), RESPONSE_FLAGS);
		this->setTimeout(REQUEST_DEFAULT_CGI_TIMEOUT);
		this->_cgi._start();
	}
	else if (this->_state == Request::FINISH)
	{
		this->_timeout = 0;
		modifySocketEpoll(g_server.getEpollFD(), this->_client->getFd(), RESPONSE_FLAGS);
	}
}

/*
** @brief Set the header state
*/
void	Request::_setHeaderState(void)
{
	if (this->_findServer() == -1 || this->_findLocation() == -1)
		return ;
	if (this->_checkTransferEncoding() == -1 || this->_checkClientMaxBodySize() == -1 || this->_checkMethod() == -1 || this->_checkCgi() == -1) // || this->_checkCGI() == -1)
		return ;
}

/*
** @brief Set the error code
**
** @param code : The error code
*/
void	Request::setError(int code)
{
	this->_stateCode = code;
	this->_setState(Request::FINISH);
}

/*
** --------------------------------- PROCESS -----------------------------------
*/

/*
** @brief Process the URI
*/
int	Request::_processUri(void)
{
	if (Utils::urlDecode(this->_uri) == -1)
		return (this->setError(400), -1);
	size_t pos = this->_uri.find('?');
	if (pos != std::string::npos)
	{
		this->_path = this->_uri.substr(0, pos);
		this->_query = this->_uri.substr(pos + 1);
	}
	else
		this->_path = this->_uri;
	return (0);
}

/*
** --------------------------------- FINDERS ----------------------------------
*/

/*
** @brief Find the server
*/
int	Request::_findServer(void)
{
	if (this->_client == NULL)
	{
		Logger::log(Logger::ERROR, "[_findServer] Client is NULL");
		this->setError(500);
		return (-1);
	}
	std::string host = this->_headers["Host"]; // Find the host in the headers
	if (host.empty()) // If the host is empty, set the error code to 400
	{
		Logger::log(Logger::ERROR, "[_findServer] Host not found in headers");
		this->setError(400);
		return (-1);
	}
	
	Logger::log(Logger::DEBUG, "[_findServer] Host: %s", host.c_str());
	
	Socket* socket = this->_client->getSocket();
	if (socket == NULL)
	{
		Logger::log(Logger::ERROR, "[_findServer] Socket is NULL");
		this->setError(500);
		return (-1);
	}

	// BlocServer* serverFound = NULL;
	std::vector<BlocServer>* servers = socket->getServers();
	for (std::vector<BlocServer>::iterator it = servers->begin(); it != servers->end(); ++it)
	{
		std::vector<std::string> serverNames = it->getServerNames();
		for (std::vector<std::string>::iterator it2 = serverNames.begin(); it2 != serverNames.end(); ++it2)
		{
			if (*it2 == host.substr(0, host.find(":")))
			{
				// serverFound = &(*it);
				this->_server = &(*it);
				break ;
			}
		}
	}
	// if (serverFound == NULL) // If the server is not found, set the first BlocServer
	// {
	// 	if (servers->empty())
	// 	{
	// 		Logger::log(Logger::ERROR, "[_findServer] No server found");
	// 		this->setError(500);
	// 		return (-1);
	// 	}
	// 	serverFound = &servers->front();
	// }
	// this->_server = serverFound;
	return (1);
	
	// Logger::log(Logger::DEBUG, "[_findServer] Server found: %s", this->_server->getServerNames().front().c_str());
}

/*
** @brief Find the location
*/
int	Request::_findLocation(void)
{
	if (this->_server == NULL)
	{
		Logger::log(Logger::ERROR, "[_findLocation] Server is NULL");
		this->setError(500);
		return (-1);
	}
	
	BlocLocation* locationFound = NULL;
	std::vector<BlocLocation>* locations = this->_server->getLocations();

	int lastClosestMatch = -1;
	for (std::vector<BlocLocation>::iterator it = locations->begin(); it != locations->end(); ++it)
	{
		std::string	path = it->getPath();
		if (this->_checkPathsMatch(this->_path, path))
		{
			if ((int)path.size() > lastClosestMatch)
			{
				lastClosestMatch = path.size();
				locationFound = &(*it);
			}
		}
	}
	this->_location = locationFound;
	return (0);
}

/*
** --------------------------------- CHECKERS ---------------------------------
*/

/*
** @brief Check the transfer encoding
**
** @return 0 if the check is successful, -1 otherwise
*/
int	Request::_checkTransferEncoding(void)
{
	if (this->_headers.find("Transfer-Encoding") != this->_headers.end())
	{
		if (this->_headers["Transfer-Encoding"] == "chunked")
			this->_isChunked = true;
		else if (this->_headers["Transfer-Encoding"] != "identity")
		{
			Logger::log(Logger::ERROR, "[_checkTransferEncoding] Transfer-Encoding not supported: %s", this->_headers["Transfer-Encoding"].c_str());
			this->setError(501);
			return (-1);
		}
	}
	return (0);
}

/*
** @brief Check the client max body size
**
** @return 0 if the check is successful, -1 otherwise
*/
int	Request::_checkClientMaxBodySize(void)
{
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		std::istringstream iss(this->_headers["Content-Length"]);
		iss >> this->_contentLength;
	}
	if (this->_contentLength > (int)this->_server->getClientMaxBodySize())
	{
		Logger::log(Logger::ERROR, "[_checkClientMaxBodySize] Content-Length too big, max body size: %d, content length: %d", this->_server->getClientMaxBodySize(), this->_contentLength);
		this->setError(413);
		return -1;
	}
	return 0;
}

/*
** @brief Check the method
**
** @return 0 if the check is successful, -1 otherwise
*/
int	Request::_checkMethod(void)
{
	if (this->_location == NULL) // if the location is not found, allow all methods
		return (0);
	if (this->_location->isMethodAllowed(BlocLocation::converStrToMethod(this->_method)))
		return (0);
	Logger::log(Logger::ERROR, "[_checkMethod] Method not allowed: %s", this->_method.c_str());
	this->setError(405);
	return (-1);
}

/*
** @brief Check if the path is inside another path
**
** @param path : The path
** @param parentPath : The parent path
**
** @return 0 if the check is successful, -1 otherwise
*/
int	Request::_checkPathsMatch(const std::string &path, const std::string &parentPath)
{
	size_t	pathSize = path.size();
	size_t	parentPathSize = parentPath.size();
	if (path.compare(0, parentPathSize, parentPath) == 0)
		if (pathSize== parentPathSize || path[parentPathSize] == '/' || parentPath == "/")
			return (1);
	return (0);
}

/*
** @brief Check the CGI
**
** @return 0 if the check is successful, -1 otherwise
*/
int	Request::_checkCgi(void)
{
	if (this->_location == NULL)
		return (0);
	std::vector<std::string> allPathsLocations = this->_getAllPathsLocation();
	for (size_t i = 0; i < allPathsLocations.size(); i++){
		for (std::map<std::string, std::string>::const_iterator it = this->_location->getCGI().begin(); it != this->_location->getCGI().end(); ++it)
			if (getExtension(allPathsLocations[i]) == it->first)
				if (fileExist(allPathsLocations[i]))
					return (this->setCgi(true, allPathsLocations[i], it->second), 0);
	}
	return (0);
}

/*
** ---------------------------------- BODY -------------------------------------
*/

/*
** @brief Define the body destination
*/
void	Request::_defineBodyDestination(void)
{
	// if (1 == 0) // TODO: Condition to see if its an upload
	if (!this->_cgi._isCGI && (this->_method == "POST" || this->_method == "PUT"))
	{

		// if content type is multipart/form-data set error 415
		if (this->_headers.find("Content-Type") != this->_headers.end() && this->_headers["Content-Type"] == "multipart/form-data")
			return (this->setError(415));
		// if (this->_headers.find("Content-Type") != this->_headers.end() && this->_headers["Content-Type"] == "application/octet-stream")
		// {
		bool isPathDir = this->_path.size() > 1 && this->_path[this->_path.size() - 1] == '/';
		{
			this->_body._path = this->_location ? this->_location->getRoot() : this->_server->getRoot();
			if (!isPathDir)
			{
				this->_body._path += this->_path;
				if (this->_method == "POST" && fileExist(this->_body._path))
					this->setError(403);
				this->_body._fd = open(this->_body._path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (this->_body._fd == -1)
					this->setError(403); // TODO: Check the error code
			}
			else
			{
				this->_body._path += this->_path;
				if (this->_headers.find("Filename") != this->_headers.end())
				{
					this->_body._path += "/" + this->_headers["Filename"];
					if (this->_method == "POST" && fileExist(this->_body._path))
						this->setError(403);
					std::cout << C_RED << "OUTPUT PATH: " << this->_body._path << C_RESET << std::endl;
					this->_body._fd = open(this->_body._path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
					if (this->_body._fd == -1)
						this->setError(403); // TODO: Check the error code
				}
				// else if (this->_query.find("filename=") != std::string::npos)
				// {
				// 	this->_body._path += "/" + this->_query.substr(this->_query.find("filename=") + 9);
				// 	if (this->_method == "POST" && fileExist(this->_body._path))
				// 		this->setError(403);
				// 	this->_body._fd = open(this->_body._path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
				// 	if (this->_body._fd == -1)
				// 		this->setError(403); // TODO: Check the error code
				// }
				else
				{
					this->_body._path += "/upload_";
					if (Utils::createFileRandomSuffix(this->_body._path, this->_body._fd) == -1)
						this->setError(403); // TODO: Check the error code
				}
			}
			this->_body._isTmp = false;
		}
		// else
		// 	this->setError(415);
	}
	else
	{
		if (Utils::createTmpFile(this->_body._path, this->_body._fd) == -1)
			this->setError(500); // TODO: Check the error code
		this->_body._isTmp = true;
	}

}

/*
** --------------------------------- TIMEOUT -----------------------------------
*/

/*
** @brief Init the timeout
*/
void	Request::_initTimeout(void)
{
	time_t currentTime = time(NULL);
	this->_timeout = currentTime + REQUEST_DEFAULT_HEADER_TIMEOUT;
}

/*
** @brief Check the timeout
**
** @param epollfd : The epoll file descriptor
*/
void	Request::checkTimeout(void)
{
	if (this->_timeout == 0 || this->_state == Request::FINISH)
		return ;
	time_t currentTime = time(NULL);
	if (currentTime > this->_timeout)
	{
		Logger::log(Logger::ERROR, "[checkTimeout] Client %d timeout", this->_client->getFd());
		// if its during cgi kill the process
		if (this->_state >= Request::CGI_INIT)
		{
			this->_cgi._kill();
			this->setError(504);
		}
		else
			this->setError(408);
		// modifySocketEpoll(epollfd, this->_client->getFd(), RESPONSE_FLAGS);
	}
}

/*
** --------------------------------- INIT -------------------------------------
*/

/*
** @brief Init the server
*/
void	Request::_initServer(void)
{
	if (this->_client == NULL)
	{
		Logger::log(Logger::ERROR, "[_initServer] Client is NULL");
		this->setError(500);
		return ;
	}
	Socket* socket = this->_client->getSocket();
	if (socket == NULL)
	{
		Logger::log(Logger::ERROR, "[_initServer] Socket is NULL");
		this->setError(500);
		return ;
	}
	std::vector<BlocServer>* servers = socket->getServers();
	if (servers->empty())
	{
		Logger::log(Logger::ERROR, "[_initServer] No server found");
		this->setError(500);
		return ;
	}
	this->_server = &servers->front();
}

/*
** --------------------------------- TOOLS ---------------------------------
*/

std::vector<std::string> Request::_getAllPathsLocation(void)
{
	if (this->_location == NULL)
		return std::vector<std::string>();

	std::vector<std::string> allPathsLocations;
	std::string path = this->_path;
	std::string root = this->_location->getRoot();
	std::string alias = this->_location->getAlias();
	std::vector<std::string> indexes = this->_location->getIndexes();
	bool isAlias = false;


	if (root.empty())
		root = this->_server->getRoot();

	if (!alias.empty())
	{
		root = alias;
		isAlias = true;
	}

	// cas ou la requete demande un fichier direct
	if (path[path.size() - 1] != '/'){
		if (isAlias)
			path = path.substr(this->_location->getPath().size());
		allPathsLocations.push_back(root + path);
	}

	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string index = indexes[i];
		if (path == "/"){
			path = root + "/" + index;
		}
		else if (isAlias){
			path = root + "/" + index;
		}
		else{
			path = root + path + index;
		}
		allPathsLocations.push_back(path);
	}

	return allPathsLocations;
}