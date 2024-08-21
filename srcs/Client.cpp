#include "Client.hpp"

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

Client::Client(void) : _fd(-1), _socket(NULL), _request(new Request(this)), _response(NULL)
{
}

Client::Client(int fd, Socket* socket) : _socket(socket), _request(new Request(this)), _response(NULL)
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
	if (this->_request != NULL)
		delete this->_request;
	if (this->_response != NULL)
		delete this->_response;
}

/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the request of the client
 * 
 */
int	Client::handleRequest( int epollFD )
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

	if (this->_request->getState() == Request::FINISH)
		this->handleResponse(epollFD);

	return (bytesRead);
}


/**
 * @brief Handle the response of the client
 * 
 */
void Client::handleResponse(int epollFD)
{
	this->_response = new Response();
	// this->_response = Response(this->_request, ICI IL ME FAUT LE BLOC SERVER);

	// mettre le socket en epollout car on a une reponse a envoyer
	modifySocketEpoll(epollFD, this->_fd, RESPONSE_FLAGS);
}

/**
* Une fois qu'on a envoye la reponse, il faut clear la requete, a voir comment faire ca clean ?
 */
void Client::clearRequest(void)
{
	delete this->_request;
	this->_request = new Request(this);
}
