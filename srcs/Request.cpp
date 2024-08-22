#include "Request.hpp"

Request::Request(void) : _client(NULL), _server(NULL), _rawRequest(""), _method(""), _uri(""), _httpVersion(""), _body(""), _version(""), _isChunked(false), _contentLength(-1), _chunkSize(-1), _state(Request::INIT), _stateCode(0)
{

}

Request::Request(Client *client) : _client(client), _server(NULL),  _rawRequest(""), _method(""), _uri(""), _httpVersion(""), _body(""), _version(""), _isChunked(false), _contentLength(-1),  _chunkSize(-1), _state(Request::INIT), _stateCode(0)
{
}

Request::Request(const Request &src)
{
	*this = src;
}

Request::~Request(void)
{
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_rawRequest = rhs._rawRequest;
		this->_method = rhs._method;
		this->_uri = rhs._uri;
		this->_httpVersion = rhs._httpVersion;
		this->_body = rhs._body;
		this->_version = rhs._version;
		this->_headers = rhs._headers;
		this->_isChunked = rhs._isChunked;
		this->_contentLength = rhs._contentLength;
		this->_chunkSize = rhs._chunkSize;
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
	if (rawRequest.empty())
	{
		Logger::log(Logger::WARNING, "Empty request");
		return ;
	}
	this->_rawRequest += rawRequest;

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
	if (this->_state > Request::INIT)
		return (Logger::log(Logger::DEBUG, "Request line already parsed"));
	std::size_t pos = this->_rawRequest.find("\r\n");
	if (pos == std::string::npos)
		return (Logger::log(Logger::DEBUG, "Incomplete request line, waiting for more data"));
	std::string 		line = this->_rawRequest.substr(0, pos);
	std::istringstream	iss(line);
	if (!(iss >> this->_method >> this->_uri >> this->_httpVersion))
	{
		this->_setError(400);
		return (Logger::log(Logger::ERROR, "Error parsing request line"));
	}
	this->_method.erase(std::remove_if(this->_method.begin(), this->_method.end(), ::isspace), this->_method.end());
	this->_uri.erase(std::remove_if(this->_uri.begin(), this->_uri.end(), ::isspace), this->_uri.end());
	this->_httpVersion.erase(std::remove_if(this->_httpVersion.begin(), this->_httpVersion.end(), ::isspace), this->_httpVersion.end());

	Logger::log(Logger::DEBUG, "Method: %s, URI: %s, HTTP Version: %s", this->_method.c_str(), this->_uri.c_str(), this->_httpVersion.c_str());
	this->_rawRequest.erase(0, pos + 2); // Remove the request line from the raw request
	this->_setState(Request::HEADERS);
}

/*
** @brief Parse the headers
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void	Request::_parseHeaders(void)
{
	if (this->_state < Request::HEADERS)
		return (Logger::log(Logger::DEBUG, "Request line not parsed yet"));
	if (this->_state > Request::HEADERS)
		return (Logger::log(Logger::DEBUG, "Headers already parsed"));

	std::size_t	pos = this->_rawRequest.find("\r\n");
	if (pos == std::string::npos)
		return (Logger::log(Logger::DEBUG, "Incomplete headers, waiting for more data"));
	while (pos != std::string::npos)
	{
		std::string 		line = this->_rawRequest.substr(0, pos);
		std::istringstream	iss(line);
		if (line.empty()) // Empty line, end of headers
		{
			this->_rawRequest.erase(0, pos + 2); // Remove the empty line from the raw request
			this->_setState(Request::BODY);
			return ;
		}

		std::size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
		{
			this->_setError(400); // TODO : Set right error code
			return (Logger::log(Logger::ERROR, "Malformed header: %s", line.c_str()));
		}
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
		value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

		this->_headers[key] = value;
		this->_rawRequest.erase(0, pos + 2); // Remove the line from the raw request
		pos = this->_rawRequest.find("\r\n");
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

	Logger::log(Logger::DEBUG, "Expected content length: %d", this->_contentLength);
	this->_body += this->_rawRequest.substr(0, this->_contentLength - this->_body.size());
	this->_rawRequest.erase(0, this->_contentLength - this->_body.size());
	if ((int)this->_body.size() == this->_contentLength)
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
			std::cout << "Line: " << line << std::endl;
			std::istringstream iss(line);
			if (!(iss >> std::hex >> this->_chunkSize))
			{
				this->_setError(400);
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
			this->_setError(400);
			return (Logger::log(Logger::ERROR, "[_parseChunkedBody] Chunk size does not match"));
		}
		this->_body += this->_rawRequest.substr(0, this->_chunkSize);
		this->_rawRequest.erase(0, this->_chunkSize + 2);
		this->_chunkSize = -1;
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
	this->_state = state;

	if (state == Request::FINISH)
	{
		if (this->_stateCode != 0)
			Logger::log(Logger::DEBUG, "[_setState] Request state changed to FINISH with error code: %d", this->_stateCode);
		else
			Logger::log(Logger::DEBUG, "[_setState] Request state changed to FINISH");
	}
	else if (state == Request::INIT)
		Logger::log(Logger::DEBUG, "[_setState] Request state changed to INIT");
	else if (state == Request::HEADERS)
		Logger::log(Logger::DEBUG, "[_setState] Request state changed to HEADERS");
	else if (state == Request::BODY)
	{
		this->_setHeaderState(); // Set the header state
		if (this->_state == Request::FINISH)
			return ;
		if (this->_method != "POST" && this->_method != "PUT")
			this->_setState(Request::FINISH);
		else
			Logger::log(Logger::DEBUG, "[_setState] Request state changed to BODY");
	}
	else
		Logger::log(Logger::DEBUG, "Invalid state: %d\n", state);
}

/*
** @brief Set the header state
*/
void	Request::_setHeaderState(void)
{
	if (this->_headers.find("Transfer-Encoding") != this->_headers.end() && this->_headers["Transfer-Encoding"] == "chunked")
		this->_isChunked = true;
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		std::istringstream iss(this->_headers["Content-Length"]);
		iss >> this->_contentLength;
	}

	this->_findServer();
	if (this->_server == NULL)
	{
		Logger::log(Logger::ERROR, "[_setHeaderState] Server is NULL");
		this->_setError(500);
		return ;
	}
	if (this->_contentLength > (int)this->_server->getClientMaxBodySize())
	{
		Logger::log(Logger::ERROR, "[_setHeaderState] Content-Length too big, max body size: %d, content length: %d", this->_server->getClientMaxBodySize(), this->_contentLength);
		this->_setError(413);
		return ;
	}
}

/*
** @brief Set the error code
**
** @param code : The error code
*/
void	Request::_setError(int code)
{
	this->_stateCode = code;
	this->_setState(Request::FINISH);
}

/*
** @brief Find the server
*/
void	Request::_findServer(void)
{
	if (this->_client == NULL)
	{
		Logger::log(Logger::ERROR, "[_findServer] Client is NULL");
		this->_setError(500);
		return ;
	}
	if (this->_server != NULL)
	{
		Logger::log(Logger::DEBUG, "[_findServer] Server already set");
		return ;
	}

	std::string host = this->_headers["Host"]; // Find the host in the headers
	if (host.empty()) // If the host is empty, set the error code to 400
	{
		Logger::log(Logger::ERROR, "[_findServer] Host not found in headers");
		this->_setError(400);
		return ;
	}
	
	Logger::log(Logger::DEBUG, "[_findServer] Host: %s", host.c_str());
	
	Socket* socket = this->_client->getSocket();
	if (socket == NULL)
	{
		Logger::log(Logger::ERROR, "[_findServer] Socket is NULL");
		this->_setError(500);
		return ;
	}

	BlocServer* serverFound = NULL;
	std::vector<BlocServer>* servers = socket->getServers();
	for (std::vector<BlocServer>::iterator it = servers->begin(); it != servers->end(); ++it)
	{
		std::vector<std::string> serverNames = it->getServerNames();
		for (std::vector<std::string>::iterator it2 = serverNames.begin(); it2 != serverNames.end(); ++it2)
		{
			if (*it2 == host)
			{
				serverFound = &(*it);
				break ;
			}
		}
	}
	if (serverFound == NULL) // If the server is not found, set the first BlocServer
		serverFound = &servers->front();
	this->_server = serverFound;
	Logger::log(Logger::DEBUG, "[_findServer] Server found: %s", this->_server->getLocations().front().getRoot().c_str());
}
