#include "Request.hpp"

// Request::Request(void) : _client(NULL), _server(NULL), _location(NULL), _rawRequest(""), _method(""), _uri(""), _path(""), _httpVersion(""), _body(""), _bodySize(0), _isChunked(false), _contentLength(-1), _chunkSize(-1), _state(Request::INIT), _stateCode(REQUEST_DEFAULT_STATE_CODE)
// {

// }

std::string	Request::getParseStateStr(e_parse_state state) const
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
		case BODY:
			return "BODY";
		case FINISH:
			return "FINISH";
		default:
			return "UNKNOWN";
	}
}

Request::Request(Client *client) : _client(client), _server(NULL), _location(NULL),  _rawRequest(""), _method(""), _uri(""), _path(""), _httpVersion(""), _body(""), _bodySize(0), _isChunked(false), _contentLength(-1),  _chunkSize(-1), _timeout(0), _state(Request::INIT), _stateCode(REQUEST_DEFAULT_STATE_CODE)
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
	while (i < rawSize && this->_rawRequest[i])
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
	while (i < rawSize && this->_rawRequest[i])
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
		Logger::log(Logger::DEBUG, "URI: %s", this->_uri.c_str());
		this->_processUri();
		this->_setState(Request::REQUEST_LINE_HTTP_VERSION);
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
	while (i < rawSize && this->_rawRequest[i])
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
			return (this->_setState(Request::BODY));
		}
		if (this->_rawRequest.size() < 2)
			return ;
		if (this->_rawRequest[1] == '\n')
		{
			this->_rawRequest.erase(0, 2);
			if (!this->_tmpHeaderKey.empty())
				return (this->setError(400));
			return (this->_setState(Request::BODY));
		}
		return (this->setError(400));
	}
	size_t	i = 0;
	size_t	rawSize = this->_rawRequest.size();
	bool	found = false;
	while (i < rawSize && this->_rawRequest[i])
	{
		if (this->_rawRequest[i] == ' ' || this->_rawRequest[i] == '\t')
			return (this->setError(400));
		if (this->_rawRequest[i] == ':')
		{
			found = true;
			break;
		}
		if (!std::isprint(this->_rawRequest[i]))
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
	while (i < rawSize && this->_rawRequest[i])
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
	if (this->_state < Request::BODY)
		return (Logger::log(Logger::DEBUG, "Headers not parsed yet"));
	if (this->_state > Request::BODY)
		return (Logger::log(Logger::DEBUG, "Body already parsed"));

	if (this->isChunked())
		return (this->_parseChunkedBody());

	std::string newContent = this->_rawRequest.substr(0, this->_contentLength - this->_bodySize);
	this->_body += newContent;
	this->_bodySize += newContent.size();
	this->_rawRequest.erase(0, this->_contentLength - this->_bodySize);
	if ((int)_bodySize == this->_contentLength)
		return (this->_setState(Request::FINISH));
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
				return (this->_setState(Request::FINISH));
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
		// std::string newContent = this->_rawRequest.substr(0, this->_chunkSize);
		// if (this->_file.getFile()->is_open())
		// {
		// 	if (!(this->_file.getFile()->write(newContent.c_str(), newContent.size())))
		// 		return (this->_uploadFailed());
		// }
		// else
			// this->_body += newContent;
		this->_body += this->_rawRequest.substr(0, this->_chunkSize);
		this->_bodySize += this->_chunkSize;
		this->_rawRequest.erase(0, this->_chunkSize + 2);
		this->_chunkSize = -1;
		if (this->_bodySize > (size_t)this->_server->getClientMaxBodySize()) // Check the client max body size
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

	if (state == Request::BODY)
	{
		this->setTimeout(REQUEST_DEFAULT_BODY_TIMEOUT);
		this->_setHeaderState(); // Set the header state
	}
	else if (state == Request::FINISH)
		this->_timeout = 0;
	if (this->_state == Request::FINISH)
		return ;
	
	this->_state = state;
}

/*
** @brief Set the header state
*/
void	Request::_setHeaderState(void)
{
	if (this->_findServer() == -1 || this->_findLocation() == -1)
		return ;
	if (this->_checkTransferEncoding() == -1 || this->_checkClientMaxBodySize() == -1 || this->_checkMethod() == -1) // || this->_checkCGI() == -1)
		return ;
	
	if (this->_method != "POST" && this->_method != "PUT")
		this->_setState(Request::FINISH);
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
void	Request::_processUri(void)
{
	size_t pos = this->_uri.find('?');
	if (pos != std::string::npos)
	{
		this->_path = this->_uri.substr(0, pos);
		this->_query = this->_uri.substr(pos + 1);
	}
	else
		this->_path = this->_uri;
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
** @brief Find the filename
**
** @return The filename
*/
// std::string	Request::_findFilename(void)
// {
// 	std::string filename;
// 	// search in headers
// 	if (this->_headers.find("Content-Disposition") != this->_headers.end())
// 	{
// 		std::string contentDisposition = this->_headers["Content-Disposition"];
// 		size_t pos = contentDisposition.find("filename=");
// 		if (pos != std::string::npos)
// 		{
// 			filename = contentDisposition.substr(pos + 9);
// 			pos = filename.find(";");
// 			if (pos != std::string::npos)
// 				filename = filename.substr(0, pos);
// 		}
// 	}
// 	// search in query
// 	else if (this->_query.find("filename") != this->_query.end())
// 		filename = this->_query["filename"];
// 	// search in path
// 	else
// 	{
// 		size_t pos = this->_path.find_last_of('/');
// 		filename = this->_path.substr(pos + 1);
// 	}

// 	if (filename.empty())
// 		filename = this->_getRandomFilename();
	
// 	this->_file.setPath(REQUEST_DEFAULT_UPLOAD_PATH + this->_path + filename);

// 	return (filename);
// }

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
void	Request::checkTimeout(int epollfd)
{
	if (this->_timeout == 0 || this->_state == Request::FINISH)
		return ;
	time_t currentTime = time(NULL);
	if (currentTime > this->_timeout)
	{
		Logger::log(Logger::ERROR, "[checkTimeout] Client %d timeout", this->_client->getFd());
		this->setError(408);
		modifySocketEpoll(epollfd, this->_client->getFd(), RESPONSE_FLAGS);
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
