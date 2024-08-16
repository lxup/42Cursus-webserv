#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server() : _isRunning(true), _epollFD(-1)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

/**
 * @brief Destroy the Server:: Server object
 * 
 * - close the epoll instance
 */
Server::~Server(){
	if (_epollFD != -1)
		close(_epollFD);
	for (std::map<std::string, int>::iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it) {
		if (it->second != -1)
			close(it->second);
	}
}

/*
** -------------------------------- METHODS --------------------------------
*/

void Server::stop( void ) {
   _isRunning = false;
}


/**
 * @param ret return code of function to test
 * @param msg message in case of error
 * @return the return code of the function if >= 0
 */
int Server::check(int ret, std::string msg)
{
	if (ret < 0){
		throw WebservException(Logger::FATAL, msg.c_str());
	}
	return ret;
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
	check(epoll_ctl(_epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function");
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
	check(epoll_ctl(_epollFD, EPOLL_CTL_MOD, sockFD, &ev), "Error with epoll_ctl function");
}

void Server::deleteSocketEpoll(int sockFD)
{
	epoll_event ev;
	ev.data.fd = sockFD;
	check(epoll_ctl(_epollFD, EPOLL_CTL_DEL, sockFD, &ev), "Error with epoll_ctl function");
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
void Server::init(std::vector<BlocServer> serversConfig)
{
	_serversConfig = serversConfig;
	_epollFD = check(epoll_create1(O_CLOEXEC), "Error with epoll function");

	for (size_t i = 0; i < serversConfig.size(); i++)
	{
		BlocServer blocServer = serversConfig[i];
		std::string ipPort = blocServer.getIpPortJoin();
		if (_listeningSockets.find(ipPort) == _listeningSockets.end())
		{
			//ajouter un new socket
			int sockFD = check(socket(AF_INET, SOCK_STREAM, 0), "Error with function socket");

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(blocServer.getIp().c_str());
			addr.sin_port = htons(blocServer.getPort());

			//int optval = 1;
			//check(setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)), "Error with setsockopt SO_REUSEADDR");

			check(bind(sockFD, (const sockaddr *)(&addr), sizeof(addr)), "Error with function bind");

			check(fcntl(sockFD, F_SETFL, O_NONBLOCK), "Error with function fcntl");

			check(listen(sockFD, BACKLOGS), "Error with function listen");

			_listeningSockets[ipPort] = sockFD;
 
			addSocketEpoll(sockFD, EPOLLIN);
		}
	}
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


/**
 * 
 * If fd corresponds to a listening socket, it means
 * that it is a new connection. Otherwise, it is a known client.
 * 
 * @param fd The file descriptor to check.
 * @return True if it is a new connection, false otherwise.
 */
bool Server::isNewConnection(int fd){
	std::map<std::string, int>::iterator it;
	for (it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it){
		if (it->second == fd)
			return true;
	}
	return false;
}

void Server::closeConnection(int fd){
	Logger::log(Logger::DEBUG, "Connection closed with client %d", fd);
	close(fd);
	deleteSocketEpoll(fd);
}

void Server::sendResponse(int fd){
	// envoi temporaire d'une reponse hardcode
	std::string body = "<h1>Hello, World bonjour louis!</h1>";
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(body.size()) + "\n\n" + body;

	check((send(events[i].data.fd, response.c_str(), response.size(), 0)), "Error with function send");
	modifySocketEpoll(fd, REQUEST_FLAGS);
	Logger::log(Logger::DEBUG, "Sending response to client %d", fd);
	// send response
	// closeConnection(fd);
}


/**
 * @brief Handle the event of a file descriptor
 * 
 * @param fd The file descriptor to handle.
 * @param event The event to handle.
 */
void Server::handleEvent(int fd, uint32_t event){
		if (event & EPOLLIN){
			if (isNewConnection(fd)){
				// nouvelle connexion, faut l'ajouter a epoll
				Logger::log(Logger::DEBUG, "New connection to the epoll");
				int connSock = check(accept(fd, NULL, NULL), "Error with accept function");
				check(fcntl(connSock, F_SETFL, O_NONBLOCK), "Error with function fcntl");
				showIpPortClient(connSock);
				addSocketEpoll(connSock, REQUEST_FLAGS);
			}else {
				// client qu'on connait deja
				Logger::log(Logger::DEBUG, "Known client, on gere la requete");
				handleConnection(fd);
			}
		} else if (event & EPOLLOUT) {
			sendResponse(fd);
		} else if (event & EPOLLRDHUP || event & EPOLLERR) {
			closeConnection(fd);
		}
}

/**
 * @brief Main loop of Webserv
 * listen with epoll_wait an event and then handle it
 * either it's a new connection either it's already a knowned client	
 * 
 */
void Server::run( void ){
	epoll_event events[MAX_EVENTS];

	while (_isRunning)
	{
		Logger::log(Logger::INFO, "Waiting for connections...");
		int nfds = check(epoll_wait(_epollFD, events, MAX_EVENTS, -1), "Error with function epoll wait");
		Logger::log(Logger::DEBUG, "There are %d file descriptors ready for I/O after epoll wait", nfds);
		for (int i = 0; i < nfds; i++)
		{
			int fd = events[i].data.fd;
			uint32_t = events[i].event;
			printEvent(fd, event);
			handle_event(fd, event);
		}
	}
	
}
