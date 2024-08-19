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
	// Close all the listening sockets
	for (std::map<int, Socket>::iterator it = this->_sockets.begin(); it != this->_sockets.end(); ++it)
	{
		if (it->second.getFd() != -1)
			protectedCall(close(it->second.getFd()), "Faild to close listening socket", false);
	}
	this->_sockets.clear();
	// Close all the client sockets
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->second.getFd() != -1)
			protectedCall(close(it->second.getFd()), "Faild to close client socket", false);
	}
	this->_clients.clear();
}

/*
** -------------------------------- METHODS --------------------------------
*/

void Server::stop( void ) {
	this->setState(S_STATE_STOP);
}

/**
 * WARNING: this function do not update flags for a socket, it only add socket to 
 * epoll with flags
 */
void Server::addSocketEpoll(int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(_epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function");
}

/**
 * @brief
 * WARNING: this function over written previous flags
 * 
 */
void Server::modifySocketEpoll(int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(_epollFD, EPOLL_CTL_MOD, sockFD, &ev), "Error with epoll_ctl function");
}

void Server::deleteSocketEpoll(int sockFD)
{
	epoll_event ev;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(_epollFD, EPOLL_CTL_DEL, sockFD, &ev), "Error with epoll_ctl function");
}

// void Server::updateSocketEpoll(int sockFD, uint32_t flags, int op){
// 	epoll_event ev;

// 	if (operation == EPOLL_CTL_MOD || operation == EPOLL_CTL_ADD) {
//         ev.events |= flags; // Ajouter les nouveaux flags aux existants
//     } else if (operation == EPOLL_CTL_DEL) {
//         ev.events &= ~flags; // Supprimer les flags spécifiés
//     } else {
// 		ev.events = flags;
// 	}
// 	ev.data.fd = sockFD;
// 	check(epoll_ctl(_epollFD, op, sockFD, &ev), "Error with epoll_ctl function");
// }


/**
 * @brief Initializes the server with the given server configurations.
 * 
 * This function sets up the server by creating and binding listening sockets for 
 * each GROUP of CORRESPONDING IP:PORT 
 * create the epoll instance and add all the listening socket to the epoll
 * 
 * @param serversConfig A vector of BlocServer objects representing the server configurations.
 */
void Server::init(std::map<std::string, std::vector<BlocServer> > servers)
{
	Logger::log(Logger::DEBUG, "Create epoll instance...");
	this->setEpollFD(protectedCall(epoll_create1(O_CLOEXEC), "Failed to create epoll instance"));

	Logger::log(Logger::DEBUG, "#==============================#");
	Logger::log(Logger::DEBUG, "|| Create listening sockets...||");
	Logger::log(Logger::DEBUG, "#==============================#");

	for (std::map<std::string, std::vector<BlocServer> >::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		Socket socket(extractIp(it->first), extractPort(it->first), it->second);
		this->_sockets[socket.getFd()] = socket;
		this->addSocketEpoll(socket.getFd(), EPOLLIN);
	}



	// for (std::vector<BlocServer>::iterator it = serversConfig.begin(); it != serversConfig.end(); ++it)
	// {
	// 	// Socket	socket(*it);
	// 	this->_sockets[socket.getFd()] = socket;
	// 	this->addSocketEpoll(socket.getFd(), EPOLLIN);
	// }
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

/**
 * @brief fonction temporaire pour affichier la requete 
 * et direct renvoyer une reponse fait a la mains
 * 
 * @param clientFD 
 */
void Server::handleConnection(int clientFD){
	
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead;
	memset(&buffer, 0, BUFFER_SIZE);
	bool isEnter = false;
	
	while ((bytesRead = read(clientFD, buffer, BUFFER_SIZE - 1)) > 0)
	{
		isEnter = true;
		Logger::log(Logger::INFO, "Le message fait: %d characteres", bytesRead);
		Logger::log(Logger::INFO, buffer);
		if (buffer[bytesRead - 1] == '\n')
			break;
		memset(&buffer, 0, BUFFER_SIZE);
	}

	if (!isEnter){
		Logger::log(Logger::DEBUG, "Le client se barre, ce batard");
		close(clientFD);
		return ;
	}

	// std::string response = "Salut du mini BlocServer de Raf\n";


	// parse ta requete
	// tu genere ta response

	// tu check que tu puisse ecrire avec EPOLLOUT sur le socket 
	// tu envois ta reponse

	


	// une fois que l'on a gere la requete et genere la reponse, on passe en mode reponse pour l'envoyer
	modifySocketEpoll(clientFD, RESPONSE_FLAGS);
}


void Server::closeConnection(int fd){
	Logger::log(Logger::DEBUG, "Connection closed with client %d", fd);
	close(fd);
	deleteSocketEpoll(fd);
}

// void Server::sendResponse(int fd){
// 	// envoi temporaire d'une reponse hardcode
// 	std::string body = "<h1>Hello, World bonjour louis!</h1>";
// 	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(body.size()) + "\n\n" + body;

// 	check((send(events[i].data.fd, response.c_str(), response.size(), 0)), "Error with function send");
// 	modifySocketEpoll(fd, REQUEST_FLAGS);
// 	Logger::log(Logger::DEBUG, "Sending response to client %d", fd);
// 	// send response
// 	// closeConnection(fd);
// }


/**
 * @brief Handle the event of a file descriptor
 * 
 * @param fd The file descriptor to handle.
 * @param event The event to handle.
 */
// void Server::handleEvent(int fd, uint32_t event){
// 		if (event & EPOLLIN){
// 			if (isNewConnection(fd)){
// 				// nouvelle connexion, faut l'ajouter a epoll
// 				Logger::log(Logger::DEBUG, "New connection to the epoll");
// 				int connSock = protectedCall(accept(fd, NULL, NULL), "Error with accept function");
// 				protectedCall(fcntl(connSock, F_SETFL, O_NONBLOCK), "Error with function fcntl");
// 				showIpPortClient(connSock);
// 				addSocketEpoll(connSock, REQUEST_FLAGS);
// 			}else {
// 				// client qu'on connait deja
// 				Logger::log(Logger::DEBUG, "Known client, on gere la requete");
// 				handleConnection(fd);
// 			}
// 		} else if (event & EPOLLOUT) {
// 			// sendResponse(fd);
// 		} else if (event & EPOLLRDHUP || event & EPOLLERR) {
// 			closeConnection(fd);
// 		}
// }


/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the new client connection
 * 
 * @param fd The file descriptor of the new client.
 */
void	Server::_handleClientConnection(int fd)
{
	Logger::log(Logger::DEBUG, "New client connected on file descriptor %d", fd);
	Client client(fd, &(this->_sockets[fd]));
	this->_clients[client.getFd()] = client;
	addSocketEpoll(client.getFd(), EPOLLIN);
}

/**
 * @brief Handle the client disconnection
 * 
 */
void	Server::_handleClientDisconnection(int fd)
{
	Logger::log(Logger::DEBUG, "Client disconnected on file descriptor %d", fd);
	this->_clients.erase(fd);
	deleteSocketEpoll(fd);
	close(fd);
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
		Logger::log(Logger::DEBUG, "There are %d file descriptors ready for I/O after epoll wait", nfds);
		
		for (int i = 0; i < nfds; i++)
		{
			int fd = events[i].data.fd;
			uint32_t event = events[i].events;
			
			if (!(event & EPOLLIN) || event & EPOLLERR || event & EPOLLHUP)
			{
				Logger::log(Logger::DEBUG, "Something went wrong with file descriptor %d", fd);
			}
			if (event & EPOLLIN)
			{
				if (this->_clients.find(fd) == this->_clients.end()) // New client connection
					_handleClientConnection(fd);
				else
					if (!(this->_clients[fd].handleRequest()))
						_handleClientDisconnection(fd);
			}
			if (event & EPOLLOUT)
			{
				Logger::log(Logger::DEBUG, "EPOLLOUT event detected on file descriptor %d", fd);
			}
		}
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

/**
 * @brief affiche l'event qui a ete detecte
 */
void printEvent(int fd, uint32_t event){
	if (event & EPOLLIN)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLIN | FD: %d", fd);
	if (event & EPOLLOUT)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLOUT | FD: %d", fd);
	if (event & EPOLLRDHUP)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLRDHUP | FD: %d", fd);
	if (event & EPOLLERR)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLERR | FD: %d", fd);
	else
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: UNKNOWN | FD: %d", fd);
}

