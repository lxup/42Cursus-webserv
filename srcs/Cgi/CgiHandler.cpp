#include "CgiHandler.hpp"

std::string	CgiHandler::getStateStr(e_cgi_handler_state state)
{
	switch (state)
	{
		case INIT:
			return "INIT";
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


CgiHandler::CgiHandler(Response* response) : _response(response), _isChunked(false), _state(CgiHandler::INIT)
{
}

CgiHandler::CgiHandler(const CgiHandler &src)
{
	*this = src;
}

CgiHandler::~CgiHandler(void)
{
}

CgiHandler &CgiHandler::operator=(const CgiHandler &src)
{
	if (this != &src)
	{
		this->_response = src._response;
		this->_output = src._output;
		this->_headers = src._headers;
		this->_tmpHeaderKey = src._tmpHeaderKey;
		this->_tmpHeaderValue = src._tmpHeaderValue;
		this->_isChunked = src._isChunked;
		this->_state = src._state;
	}
	return *this;
}

/*
** --------------------------------- PARSING ---------------------------------
*/

/*
** @brief Set the parse state
**
** @param state : The state
**
** @return void
*/
void	CgiHandler::_setState(e_cgi_handler_state state)
{
	if (this->_state == CgiHandler::FINISH)
		return ;
	if (this->_state == state)
		return ;
	Logger::log(Logger::DEBUG, "CgiHandler state: %s -> %s", this->getStateStr(this->_state).c_str(), this->getStateStr(state).c_str());
	this->_state = state;

	if (this->_state == CgiHandler::BODY)
	{
		if (this->_headers.find("content-length") == this->_headers.end())
		{
			this->_isChunked = true;
			this->_headers["transfer-encoding"] = "chunked";
		}
		if (this->_checkHeaders() == -1)
			return ;
		this->_response->_response += this->_response->_request->getHttpVersion() + " " + intToString(this->_response->_request->getStateCode()) + " " + getErrorMessage(this->_response->_request->getStateCode()) + "\r\n";
		for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
			this->_response->_response += it->first + ": " + it->second + "\r\n";
		this->_response->_response += "\r\n";
	}
	else if (this->_state == CgiHandler::FINISH)
		return (this->_response->setState(Response::FINISH));

}

/*
** @brief Parse the CGI
**
** @param data : The data to parse
**
** @return void
*/
void	CgiHandler::_parse(const std::string &data)
{
	if (this->_state == CgiHandler::FINISH)
		return ;
	this->_output += data;
	this->_parseHeaders();
	this->_parseBody();
}

/*
** @brief Parse the headers
**
*/
void	CgiHandler::_parseHeaders(void)
{
	if (this->_state < CgiHandler::INIT)
		return (Logger::log(Logger::DEBUG, "Request line not parsed yet"));
	if (this->_state > CgiHandler::HEADERS_END)
		return (Logger::log(Logger::DEBUG, "Headers already parsed"));

	if (this->_state == CgiHandler::INIT)
		this->_state = CgiHandler::HEADERS_PARSE_KEY;
	
	if (this->_state == CgiHandler::HEADERS_PARSE_KEY)
		this->_parseHeadersKey();
	if (this->_state == CgiHandler::HEADERS_PARSE_VALUE)
		this->_parseHeadersValue();
	if (this->_state == CgiHandler::HEADERS_PARSE_END)
	{
		this->_output.erase(0, this->_output.find_first_not_of(" \t"));
		if (this->_output.empty())
			return ;
		if (this->_output[0] == '\n')
		{
			this->_output.erase(0, 1);
			this->_setState(CgiHandler::HEADERS_PARSE_KEY);
			return (this->_parseHeaders());
		}
		if (this->_output[0] == '\r')
		{
			if (this->_output.size() < 2)
				return ;
			if (this->_output[1] == '\n')
			{
				this->_output.erase(0, 2);
				this->_setState(CgiHandler::HEADERS_PARSE_KEY);
				return (this->_parseHeaders());
			}
			return (this->_response->setError(500));
		}
		return (this->_response->setError(500));
	}
}

/*
** @brief Parse the headers key
**
*/
void	CgiHandler::_parseHeadersKey(void)
{
	// detect end of headers
	if (!this->_output.empty() && (this->_output[0] == '\r' || this->_output[0] == '\n'))
	{
		if (this->_output[0] == '\n')
		{
			this->_output.erase(0, 1);
			if (!this->_tmpHeaderKey.empty())
				return (this->_response->setError(500));
			return (this->_setState(CgiHandler::BODY));
		}
		if (this->_output.size() < 2)
			return ;
		if (this->_output[1] == '\n')
		{
			this->_output.erase(0, 2);
			if (!this->_tmpHeaderKey.empty())
				return (this->_response->setError(500));
			return (this->_setState(CgiHandler::BODY));
		}
		return (this->_response->setError(500));
	}
	size_t	i = 0;
	size_t	rawSize = this->_output.size();
	bool	found = false;
	while (i < rawSize)
	{
		if (this->_output[i] == ' ' || this->_output[i] == '\t')
			return (this->_response->setError(500));
		if (this->_output[i] == ':')
		{
			found = true;
			break;
		}
		// if (!std::isprint(this->_output[i]))
		// 	return (this->_response->setError(500));
		if (!std::isalnum(this->_output[i]) && this->_output[i] != '-' && this->_output[i] != '_')
			return (this->_response->setError(500));
		this->_tmpHeaderKey += this->_output[i];
		i++;
	}
	this->_output.erase(0, found ? i + 1 : i);
	if (found)
	{
		if (this->_tmpHeaderKey.empty())
			return (this->_response->setError(500));
		this->_tmpHeaderKey.erase(std::remove_if(this->_tmpHeaderKey.begin(), this->_tmpHeaderKey.end(), ::isspace), this->_tmpHeaderKey.end());
		Logger::log(Logger::DEBUG, "Header key: %s", this->_tmpHeaderKey.c_str());
		this->_setState(CgiHandler::HEADERS_PARSE_VALUE);
	}

}

/*
** @brief Parse the headers value
**
*/
void	CgiHandler::_parseHeadersValue(void)
{
	size_t	i = 0;
	size_t	rawSize = this->_output.size();
	bool	found = false;
	while (i < rawSize)
	{
		// skip space and tab
		if (this->_tmpHeaderValue.empty() && (this->_output[i] == ' ' || this->_output[i] == '\t'))
		{
			i++;
			continue;
		}
		if (!std::isprint(this->_output[i]))
		{
			found = true;
			break;
		}
		this->_tmpHeaderValue += this->_output[i];
		i++;
	}
	this->_output.erase(0, i);
	if (found)
	{
		if (this->_tmpHeaderValue.empty())
			return (this->_response->setError(500));
		// this->_tmpHeaderValue.erase(std::remove_if(this->_tmpHeaderValue.begin(), this->_tmpHeaderValue.end(), ::isspace), this->_tmpHeaderValue.end());
		if (this->_headers.find(this->_tmpHeaderKey) != this->_headers.end())
			return (this->_response->setError(500));
		this->_headers[convertToLowercase(this->_tmpHeaderKey)] = this->_tmpHeaderValue;
		std::cout << "Header value: " << this->_headers[convertToLowercase(this->_tmpHeaderKey)] << std::endl;
		this->_tmpHeaderKey.clear();
		this->_tmpHeaderValue.clear();
		this->_setState(CgiHandler::HEADERS_PARSE_END);
	}
}

/*
** @brief Parse the body
**
*/
void	CgiHandler::_parseBody(void)
{
	if (this->_state < CgiHandler::BODY)
		return (Logger::log(Logger::DEBUG, "Headers not parsed yet"));
	if (this->_state > CgiHandler::BODY)
		return (Logger::log(Logger::DEBUG, "Body already parsed"));

	if (this->_isChunked)
	{
		std::string chunkSizeStr = intToHexa(this->_output.size()) + "\r\n";
		this->_response->_response += chunkSizeStr + this->_output + "\r\n";
	}
	else
		this->_response->_response += this->_output;
	this->_output.clear();
}

/*
** @brief Parse the headers of the CGI
**
** @return void
*/
// void	CgiHandler::_parseHeaders(void)
// {
// 	std::cout << C_CYAN << "Output: " << this->_output << C_RESET << std::endl;
// 	size_t pos = this->_output.find("\r\n\r\n");
// 	if (pos != std::string::npos)
// 	{
// 		std::string headers = this->_output.substr(0, pos + 2);
// 		this->_output= this->_output.substr(pos + 4);
// 		// trim whitespace output
// 		// this->_output.erase(remove_if(this->_output.begin(), this->_output.end(), ::isspace), this->_output.end());

// 		std::istringstream iss(headers);
// 		std::string line;
// 		while (std::getline(iss, line, '\n'))
// 		{
// 			if (line.empty())
// 				continue;
// 			// check if there is a \r at the end of the line
// 			if (line[line.size() - 1] != '\r')
// 				throw IntException(502); // TODO: check if it's should be considered as an error
// 			line.erase(line.size() - 1);
// 			size_t colonPos = line.find(":");
// 			if (colonPos == std::string::npos)
// 				throw IntException(502); // TODO: check if it's should be considered as an error
// 			std::string key = line.substr(0, colonPos);
// 			std::string value = line.substr(colonPos + 1);

// 			key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
// 			value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

// 			if (key == "Status")
// 			{
// 				int statusInt = atoi(value.c_str());
// 				this->_request->setStateCode(statusInt);
// 				if (statusInt >= 400)
// 					throw IntException(statusInt);
// 			}
// 			else
// 				this->_headers[key] = value;
// 		}
// 		this->_checkHeaders();
// 	}
// 	else
// 	{
// 		Logger::log(Logger::ERROR, "No headers found in CGI response");
// 		throw IntException(502); // TODO: check if it's the right error code
// 	}
// }

/*
** --------------------------------- CHECKER ---------------------------------
*/

/*
** @brief Check the headers of the CGI
**
** @return void
*/
int	CgiHandler::_checkHeaders(void)
{
	if (this->_headers.empty())
		return (this->_response->setError(502), -1); // TODO: check if it's the right error code
	if (this->_headers.find("status") != this->_headers.end())
	{
		int statusInt = atoi(this->_headers["status"].c_str());
		this->_response->_request->setStateCode(statusInt);
		if (statusInt >= 400)
			return (this->_response->setError(statusInt), -1);
		this->_headers.erase("status");
	}
	if (this->_headers.find("content-type") == this->_headers.end())
		return (this->_response->setError(502), -1);
	return (0);
}