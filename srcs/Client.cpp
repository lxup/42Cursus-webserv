#include "Client.hpp"

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

Client::Client(void) : _fd(-1)
{
}

Client::Client(int fd)
{
	Logger::log(Logger::DEBUG, "Initializing client with fd %d", fd);

	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);

	this->_fd = protectedCall(accept(fd, (struct sockaddr *)&addr, &addrLen), "Error with accept function");
	protectedCall(fcntl(this->_fd, F_SETFL, O_NONBLOCK), "Error with fcntl function");
}

Client::~Client(void)
{
}

void	Client::_addRequest(const std::string &request)
{
	this->_requests.push(Request(request));
}

/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the request of the client
 * 
 */
int	Client::handleRequest(void)
{
	Logger::log(Logger::DEBUG, "Handling request from client %d", this->_fd);
	
	char	buffer[CLIENT_READ_BUFFER_SIZE + 1];
	int		bytesRead = 0;

	memset(buffer, 0, CLIENT_READ_BUFFER_SIZE );
	bytesRead  = recv(this->_fd, buffer, CLIENT_READ_BUFFER_SIZE, 0);
	if (bytesRead > 0)
	{
		Logger::log(Logger::DEBUG, "Received %d bytes from client %d", bytesRead, this->_fd);
		buffer[bytesRead] = '\0';
	}
	 else if (bytesRead < 0)
	{
		Logger::log(Logger::ERROR, "Error with recv function");
		return (-1);
	}
	else if (bytesRead == 0)
		return (0);

	this->_addRequest(buffer);

	return (bytesRead);
}