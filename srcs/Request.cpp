#include "Request.hpp"

Request::Request(void) : _rawRequest(""), _method(""), _uri(""), _httpVersion(""), _body(""), _version(""), _isChunked(false), _contentLength(0), _state(Request::INIT), _errorCode(0)
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
		this->_state = rhs._state;
		this->_errorCode = rhs._errorCode;
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
		this->_parseChunkedBody();

	Logger::log(Logger::DEBUG, "Expected content length: %d", this->_contentLength);
	// add rawRequest to body with content length
	this->_body += this->_rawRequest.substr(0, this->_contentLength - this->_body.size());

	Logger::log(Logger::DEBUG, "Body: %s\n", this->_body.c_str());

	if (this->_body.size() == this->_contentLength)
		this->_setState(Request::FINISH);
	// if (step == Request::BODY)
	// {
	// 	if (this->_isChunked)
	// 	{
	// 		Logger::log(Logger::DEBUG, "[Request] Chunked body\n");
	// 		this->_parseChunkedBody(iss, line, step);
	// 	}
	// 	else
	// 	{
	// 		Logger::log(Logger::DEBUG, "[Request] Body\n");
	// 		std::ostringstream oss;
	// 		while (std::getline(iss, line))
	// 			oss << line << std::endl;
	// 		this->_body = oss.str();
	// 	}
	// 	Logger::log(Logger::DEBUG, "Body: %s\n", this->_body.c_str());
	// 	step = Request::FINISH;
	// }
	// else
	// 	Logger::log(Logger::WARNING, "Invalid step: %d\n", step);
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
}

/*
** @brief Set the state
**
** @param state : The state
*/
void	Request::_setState(e_parse_state state)	
{
	this->_state = state;

	if (state == Request::FINISH)
		Logger::log(Logger::DEBUG, "Request state changed to FINISH");
	else if (state == Request::ERROR)
		Logger::log(Logger::DEBUG, "Request state changed to ERROR");
	else if (state == Request::INIT)
		Logger::log(Logger::DEBUG, "Request state changed to INIT");
	else if (state == Request::HEADERS)
		Logger::log(Logger::DEBUG, "Request state changed to HEADERS");
	else if (state == Request::BODY)
	{
		this->_setHeaderState(); // Set the header state
		if (this->_method != "POST") // If the method is not POST, the body is empty
			this->_setState(Request::FINISH);
		else
			Logger::log(Logger::DEBUG, "Request state changed to BODY");
	}
	else
		Logger::log(Logger::DEBUG, "Invalid state: %d\n", state);
}

/*
** @brief Set the header state
*/
void	Request::_setHeaderState(void)
{
	// Check if the body is chunked
	if (this->_headers.find("Transfer-Encoding") != this->_headers.end() && this->_headers["Transfer-Encoding"] == "chunked")
		this->_isChunked = true;
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		std::istringstream iss(this->_headers["Content-Length"]);
		iss >> this->_contentLength;
	}
}

/*
** @brief Set the error code
**
** @param code : The error code
*/
void	Request::_setError(int code)
{
	this->_setState(Request::ERROR);
	this->_errorCode = code;
}
