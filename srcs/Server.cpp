#include "Server.hpp"

Server::Server() : _state(S_STATE_INIT), _epollFD(-1)
{
}

/**
 * @brief Destroy the Server:: Server object
 * 
 * - close the epoll instance
 */
Server::~Server(){
	if (this->_epollFD != -1)
		protectedCall(close(_epollFD), "Faild to close epoll instance", false);
	// Delete all the sockets
	for (std::map<int, Socket*>::iterator it = this->_sockets.begin(); it != this->_sockets.end(); ++it)
		delete it->second;
	this->_sockets.clear();
	// Delete all the clients
	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		delete it->second;
	this->_clients.clear();
}

/*
** -------------------------------- METHODS --------------------------------
*/

/**
* @brief Sent the response to the client
* @TODO : handle the case where the response is too big to be sent in one time
 */
void Server::sendResponse(Client* client){
	std::string response = client->getResponse()->getRawResponse();
	Logger::log(Logger::INFO, "Response to sent: \n%s", response.c_str());
	int bytesSent = send(client->getFd(), response.c_str(), response.length(), 0);
	
	if (bytesSent < 0)
		Logger::log(Logger::ERROR, "Error with send function");
	else
		Logger::log(Logger::DEBUG, "Sent %d bytes to client %d", bytesSent, client->getFd());

	if (client->getResponse()->getState() == Response::FINISH)
	{
		Logger::log(Logger::DEBUG, "Response sent to client %d", client->getFd());
		client->clearRequest();
		modifySocketEpoll(_epollFD, client->getFd(), REQUEST_FLAGS);
	}
}

void Server::stop( void ) {
	this->setState(S_STATE_STOP);
}

/**
 * @brief Initializes the server with the given server configurations.
 * 
 * This function sets up the server by creating and binding listening sockets for 
 * each GROUP of CORRESPONDING IP:PORT 
 * create the epoll instance and add all the listening socket to the epoll
 * 
 * @param serversConfig A vector of BlocServer objects representing the server configurations.
 */
void Server::init(void)
{
	Logger::log(Logger::DEBUG, "[Server::init] Create epoll instance...");
	this->setEpollFD(protectedCall(epoll_create1(O_CLOEXEC), "Failed to create epoll instance"));

	Logger::log(Logger::DEBUG, "#==============================#");
	Logger::log(Logger::DEBUG, "|| Create listening sockets...||");
	Logger::log(Logger::DEBUG, "#==============================#");

	std::map<std::string, std::vector<BlocServer> > &servers = this->_configParser.getServers();
	for (std::map<std::string, std::vector<BlocServer> >::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		Socket* socket = new Socket(extractIp(it->first), extractPort(it->first), &it->second);
		this->_sockets[socket->getFd()] = socket;
		addSocketEpoll(this->_epollFD, socket->getFd(), REQUEST_FLAGS);
	}
	this->setState(S_STATE_READY);
}


void Server::showIpPortClient(int clientFD){
	char clientAdress[INET_ADDRSTRLEN];
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getpeername(clientFD, (struct sockaddr*)&addr, &addr_len);
	inet_ntop(AF_INET, &(addr.sin_addr), clientAdress, INET6_ADDRSTRLEN);
	uint16_t clientPort = ntohs(addr.sin_port);
	Logger::log(Logger::DEBUG, "New connection from %s:%u, SOCKET CLIENT %d", clientAdress, clientPort, clientFD);
}


/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the new client connection
 * 
 * @param fd => The LISTENING socket who get the new client
 */
void	Server::_handleClientConnection(int fd)
{
	Logger::log(Logger::DEBUG, "[Server::_handleClientConnection] New client connected on file descriptor %d", fd);
	Client *client = new Client(fd, this->_sockets[fd]);
	this->_clients[client->getFd()] = client;
	addSocketEpoll(this->_epollFD, client->getFd(), REQUEST_FLAGS);

}

/**
 * @brief Handle the client disconnection
 * 
 */
void	Server::_handleClientDisconnection(int fd)
{
	Logger::log(Logger::DEBUG, "[Server::_handleClientDisconnection] Client disconnected on file descriptor %d", fd);
	deleteSocketEpoll(this->_epollFD, fd);
	std::map<int, Client*>::iterator it = this->_clients.find(fd);
	if (it != this->_clients.end())
	{
		delete it->second;
		this->_clients.erase(it);
	}
}


/**
 * @brief Handle the event that occured on the file descriptor
 */
void Server::handleEvent(epoll_event *events, int i){
	uint32_t event = events[i].events;
	int fd = events[i].data.fd;
	
	if (event & EPOLLIN){
		if (this->_clients.find(fd) == this->_clients.end()) // New client connection
			_handleClientConnection(fd);
		else if (!(this->_clients[fd]->handleRequest(this->_epollFD)))
				_handleClientDisconnection(fd);
	}
	if (event & EPOLLOUT){
		this->sendResponse(_clients[fd]);
	}
}


/**
 * @brief Main loop of Webserv
 * listen with epoll_wait an event and then handle it
 * either it's a new connection either it's already a knowned client	
 * 
 */
void Server::run(void)
{
	if (this->getState() != S_STATE_READY)
		Logger::log(Logger::FATAL, "Server is not ready to run");
	this->setState(S_STATE_RUN);

	epoll_event	events[MAX_EVENTS];
	while (this->getState() == S_STATE_RUN)
	{

		Logger::log(Logger::INFO, "Waiting for connections...");
		int nfds = protectedCall(epoll_wait(this->_epollFD, events, MAX_EVENTS, -1), "Error with function epoll wait");
		Logger::log(Logger::DEBUG, "[Server::run] There are %d file descriptors ready for I/O after epoll wait", nfds);
		
		for (int i = 0; i < nfds; i++)
			handleEvent(events, i);
	}
}

/*
** --------------------------------- SETTERS ---------------------------------
*/

void	Server::setState(int state)
{
	if (state == S_STATE_INIT)
		Logger::log(Logger::INFO, "Server is initializing...");
	else if (state == S_STATE_READY)
		Logger::log(Logger::INFO, "Server is ready to run...");
	else if (state == S_STATE_RUN)
		Logger::log(Logger::INFO, "Server is running...");
	else if (state == S_STATE_STOP)
		Logger::log(Logger::INFO, "Server is stopping...");
	this->_state = state;
}

