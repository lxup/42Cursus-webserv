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


void Server::stop( void )
{
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
	
	try {

		if (event & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) // Error with the file descriptor
			throw ClientDisconnectedException();
		if (event & EPOLLIN){
			if (this->_clients.find(fd) == this->_clients.end()) // New client connection
				_handleClientConnection(fd);
			else{
				this->_clients[fd]->updateLastActivity();
				// if (this->_clients[fd]->checkCgi())
				// {
				// 	Logger::log(Logger::DEBUG, "[Server::handleEvent] CGI finished on client %d", fd);
				// 	std::cout << C_RED << "CGI FINIIIIISSSSSHHHHEEEEEDDDDDDDDD" << C_RESET << std::endl;
				// 	modifySocketEpoll(this->_epollFD, fd, EPOLLOUT);
				// }
				// if (this->_clients[fd]->isCgiReady(this->_epollFD)) // Check if the CGI is ready
				// 	modifySocketEpoll(this->_epollFD, fd, EPOLLOUT);
				// else
					this->_clients[fd]->handleRequest(this->_epollFD);
			}
		}
		if (event & EPOLLOUT){
			this->_clients[fd]->updateLastActivity();
			this->_clients[fd]->checkCgi();
			if (this->_clients[fd]->getRequest() && this->_clients[fd]->getRequest()->getState() == Request::FINISH)
				this->_clients[fd]->handleResponse(this->_epollFD);
		}
	} catch (ChildProcessException &e) { // Child process error (CGI)
		std::cout << C_RED << "CGI ERROR" << C_RESET << std::endl;
		throw ChildProcessException();
	} catch (ClientDisconnectedException &e) { // Client disconnected
		this->_handleClientDisconnection(fd);
	} catch (const std::exception &e) { // Other exceptions
		Logger::log(Logger::ERROR, "[Server::handleEvent] Error with client %d : %s", fd, e.what());
		this->_handleClientDisconnection(fd);
	}
}


/**
 * @brief Check the timeouts of the clients
 * 
 */
void	Server::_checkTimeouts(time_t currentTime)
{
	(void)currentTime;
	std::map<int, Client*>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		it->second->getRequest()->checkTimeout();
		// if (currentTime - it->second->getLastActivity() > INACTIVITY_TIMEOUT)
		// {
		// 	Logger::log(Logger::DEBUG, "[Server::_checkTimeouts] Client %d timed out", it->first);
		// 	deleteSocketEpoll(this->_epollFD, it->first);
		// 	delete it->second;
		// 	this->_clients.erase(it++);
		// }
		// else if (it->second->getResponse()->getCgiHandler() != NULL && (currentTime - it->second->getResponse()->getCgiHandler()->getLastActivity() > TIMEOUT_CGI))
		// {
		// 	Logger::log(Logger::DEBUG, "[Server::_checkTimeouts] CGI Timeout on client %d", it->first);
		// 	kill(it->second->getResponse()->getCgiHandler()->getPid(), SIGTERM);
		// 	it->second->getResponse()->setError(504);
		// 	it->second->getResponse()->clearCgi();
		// }
		// else
			++it;
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

	time_t lastTimeoutCheck = time(NULL);

	epoll_event	events[MAX_EVENTS];
	while (this->getState() == S_STATE_RUN)
	{
		// Logger::log(Logger::INFO, "Waiting for connections...");
		int nfds = protectedCall(epoll_wait(this->_epollFD, events, MAX_EVENTS, 500), "Error with epoll_wait function");
		Logger::log(Logger::DEBUG, "[Server::run] There are %d file descriptors ready for I/O after epoll wait", nfds);
		
		for (int i = 0; i < nfds; i++)
			handleEvent(events, i);


		time_t currentTime = time(NULL);
		(void)lastTimeoutCheck;
		// if (currentTime - lastTimeoutCheck >= TIMEOUT_CHECK_INTERVAL)
		// {
			this->_checkTimeouts(currentTime);
		// 	lastTimeoutCheck = currentTime;
		// }
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
