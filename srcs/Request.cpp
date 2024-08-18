#include "Request.hpp"

Request::Request(void) : _rawRequest(""), _method(""), _uri(""), _httpVersion(""), _body(""), _version(""), _isChunked(false), _isReady(false)
{
}

Request::Request(const std::string &rawRequest) : _rawRequest(rawRequest), _method(""), _uri(""), _httpVersion(""), _body(""), _version(""), _isChunked(false), _isReady(false)
{
	this->_parse();
	this->_isReady = true;
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
		this->_method = rhs._method;
		this->_uri = rhs._uri;
		this->_httpVersion = rhs._httpVersion;
		this->_body = rhs._body;
		this->_version = rhs._version;
		this->_headers = rhs._headers;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** @brief Parse the raw request
*/
void	Request::_parse(void)
{
		Logger::log(Logger::DEBUG, "Parsing request: %s\n", this->_rawRequest.c_str());

		// read line by line 
		std::istringstream	iss(this->_rawRequest);
		std::string			line;
		int					step(Request::START);

		this->_parseRequestLine(iss, line, step);
		this->_parseHeaders(iss, line, step);
		this->_parseBody(iss, line, step);
}

/*
** @brief Parse the request line
**
** @param iss : The input stream
** @param step : The current step
*/
void	Request::_parseRequestLine(std::istringstream &iss, std::string &line, int &step)
{
	if (step == Request::START)
	{
		std::getline(iss, line);
		Logger::log(Logger::DEBUG, "Parsing request line: %s\n", line.c_str());

		std::istringstream	issLine(line);
		issLine >> this->_method >> this->_uri >> this->_httpVersion;
		this->_method.erase(std::remove_if(this->_method.begin(), this->_method.end(), ::isspace), this->_method.end());
		this->_uri.erase(std::remove_if(this->_uri.begin(), this->_uri.end(), ::isspace), this->_uri.end());
		this->_httpVersion.erase(std::remove_if(this->_httpVersion.begin(), this->_httpVersion.end(), ::isspace), this->_httpVersion.end());

		Logger::log(Logger::DEBUG, "Method: %s, URI: %s, HTTP Version: %s\n", this->_method.c_str(), this->_uri.c_str(), this->_httpVersion.c_str());

		step = Request::HEADERS;
	}
	else
		Logger::log(Logger::WARNING, "Invalid step: %d\n", step);
}

/*
** @brief Parse the headers
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void	Request::_parseHeaders(std::istringstream &iss, std::string &line, int &step)
{
	if (step == Request::HEADERS)
	{
		// read line by line until empty line (or white space)
		while (std::getline(iss, line) && !line.empty() && line != "\r")
		{
			std::string::size_type pos = line.find(':');
			if (pos != std::string::npos)
			{
				// Get key and value
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);

				// Trim key and value
				key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
				value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

				// Add header
				this->_headers[key] = value;
				Logger::log(Logger::DEBUG, "Header: %s: %s\n", key.c_str(), value.c_str());
			}
		}
		this->_setHeaderState();
		step = Request::BODY;
	}
	else
		Logger::log(Logger::WARNING, "Invalid step: %d\n", step);
}

/*
** @brief Parse the body
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void	Request::_parseBody(std::istringstream &iss, std::string &line, int &step)
{
	if (step == Request::BODY)
	{
		if (this->_isChunked)
		{
			Logger::log(Logger::DEBUG, "[Request] Chunked body\n");
			this->_parseChunkedBody(iss, line, step);
		}
		else
		{
			Logger::log(Logger::DEBUG, "[Request] Body\n");
			std::ostringstream oss;
			while (std::getline(iss, line))
				oss << line << std::endl;
			this->_body = oss.str();
		}
		Logger::log(Logger::DEBUG, "Body: %s\n", this->_body.c_str());
		step = Request::DONE;
	}
	else
		Logger::log(Logger::WARNING, "Invalid step: %d\n", step);
}

/*
** @brief Parse the chunked body
**
** @param iss : The input stream
** @param line : The current line
** @param step : The current step
*/
void Request::_parseChunkedBody(std::istringstream &iss, std::string &line, int &step)
{
    if (step == Request::BODY)
    {
        std::ostringstream oss;
        size_t size = 0;

        while (std::getline(iss, line))
        {
            // Remove CRLF
            Logger::log(Logger::DEBUG, "Line: %s\n", line.c_str());

            // Remove CRLF from the line
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            // Log after removing CRLF
            Logger::log(Logger::DEBUG, "Line after remove CRLF: %s\n", line.c_str());

            if (size == 0)
            {
                // Get the size of the chunk
                std::istringstream issLine(line);
                issLine >> std::hex >> size;

                if (size == 0)
                {
                    // End of the body
                    break;
                }
            }
            else
            {
                // If the remaining line size is greater than the chunk size
                if (line.size() > size)
                {
                    oss << line.substr(0, size);
                    line = line.substr(size);
                    size = 0; // Reset chunk size
                }
                else
                {
                    oss << line;
                    size -= line.size();
                }
            }
        }
        this->_body = oss.str();
    }
    else
    {
        Logger::log(Logger::WARNING, "Invalid step: %d\n", step);
    }
}


/*
** @brief Set the header state
*/
void	Request::_setHeaderState(void)
{
	// Check if the body is chunked
	if (this->_headers.find("Transfer-Encoding") != this->_headers.end() && this->_headers["Transfer-Encoding"] == "chunked")
		this->_isChunked = true;
}
