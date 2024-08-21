#include "Client.hpp"

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

Client::Client(void) : _fd(-1), _socket(NULL), _request(new Request(this))
{
}

Client::Client(int fd, Socket* socket) : _socket(socket), _request(new Request(this))
{
	Logger::log(Logger::DEBUG, "Initializing client with fd %d", fd);

	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);

	this->_fd = protectedCall(accept(fd, (struct sockaddr *)&addr, &addrLen), "Error with accept function");
	protectedCall(fcntl(this->_fd, F_SETFL, O_NONBLOCK), "Error with fcntl function");

	
}

Client::~Client(void)
{
	if (this->_fd != -1)
		protectedCall(close(this->_fd), "Faild to close client socket", false);
	delete this->_request;
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

	memset(buffer, 0, CLIENT_READ_BUFFER_SIZE + 1);
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

	this->_request->parse(buffer);

	return (bytesRead);
}


/**
* Une fois qu'on a envoye la reponse, il faut clear la requete, a voir comment faire ca clean ?
 */
void Client::clearRequest(void){
	Request newReq;
	_request = newReq;
}
