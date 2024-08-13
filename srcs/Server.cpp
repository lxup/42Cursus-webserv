#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server() : _isRunning(true)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server(){
	cleanSetup();
}

/*
** -------------------------------- METHODS --------------------------------
*/

void Server::stop( void ) {
   _isRunning = false;
}

/**
 * @brief close les fd socket ouvert
 */
void Server::cleanSetup(void)
{
	// todo : close les fd socket ouvert
	close(_epollFD);
	for (std::map<std::string, int>::iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it) {
		close(it->second);
	}
}


/**
 * @param ret return code of function to test
 * @param msg message in case of error
 * @return the return code of the function if >= 0
 */
int Server::check(int ret, std::string msg)
{
	if (ret < 0){
		cleanSetup();
		throw WebservException(Logger::FATAL, msg.c_str());
	}
	return ret;
}



void Server::addSocketEpoll(int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	check(epoll_ctl(_epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function");
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

			int optval = 1;
			check(setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)), "Error with setsockopt SO_REUSEADDR");

			check(bind(sockFD, (const sockaddr *)(&addr), sizeof(addr)), "Error with function bind");

			check(fcntl(sockFD, F_SETFL, O_NONBLOCK), "Error with function fcntl");

			check(listen(sockFD, BACKLOGS), "Error with function listen");

			_listeningSockets[ipPort] = sockFD;
 
			addSocketEpoll(sockFD, EPOLLIN);
		}
	}
}


void Server::showIpClient(int clientFD){
	// todo a modifier
	int bufferSize = 1000;
	char client_adress[INET_ADDRSTRLEN];
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getpeername(clientFD, (struct sockaddr*)&addr, &addr_len);
	inet_ntop(AF_INET, &(addr.sin_addr), client_adress, bufferSize);
	Logger::log(Logger::INFO, "Ip du client: %s", client_adress);

}

/**
 * @brief fonction temporaire pour affichier la requete 
 * et direct renvoyer une reponse fait a la mains
 * 
 * @param clientFD 
 */
void Server::handleConnection(int clientFD){
	
	char buffer[BUFFER_SIZE];
	showIpClient(clientFD);
	ssize_t bytesRead;
	memset(&buffer, 0, BUFFER_SIZE);

	while ((bytesRead = read(clientFD, buffer, BUFFER_SIZE - 1)) > 0)
	{
		Logger::log(Logger::INFO, "Le message fait: %d characteres", bytesRead);
		Logger::log(Logger::INFO, buffer);
		if (buffer[bytesRead - 1] == '\n')
			break;
		memset(&buffer, 0, BUFFER_SIZE);
	}

	// std::string response = "Salut du mini BlocServer de Raf\n";
	std::string body = "<h1>Hello, World bonjour louis!</h1>";
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(body.size()) + "\n\n" + body;

	check((send(clientFD, response.c_str(), response.size(), 0)), "Error with function send");
	close(clientFD);
	clientFD = -1;
}


/**
 * 
 * This function searches through all the listening sockets and checks if the file descriptor
 * corresponds to any of the values in the listeningSockets map. If it corresponds, it means
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

/**
 * @brief Main loop of Webserv
 * listen with epoll_wait an event and then handle it
 * either it's a new connection either it's already a knowned client	
 * 
 */
void Server::run( void ){

	// todo: a modifier
	const int MAX_EVENTS = 10;
	epoll_event events[MAX_EVENTS];

	while (_isRunning)
	{
		Logger::log(Logger::INFO, "Waiting for connections...");
		int nfds = check(epoll_wait(_epollFD, events, MAX_EVENTS, -1), "Error with function epoll wait");
		Logger::log(Logger::DEBUG, "There are %d file descriptors ready for I/O after epoll wait", nfds);
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].events & EPOLLIN){
				int fd = events[i].data.fd;
	
				if (isNewConnection(fd)){
					Logger::log(Logger::DEBUG, "New connection to the epoll, LISTENING SOCKET: %d", fd);

					// nouvelle connexion, faut l'ajouter a epoll
					//todo a checker les parametre
					int connSock = check(accept(fd, NULL, NULL), "Error with accept function");
					check(fcntl(connSock, F_SETFL, O_NONBLOCK), "Error with function fcntl");
					Logger::log(Logger::DEBUG, "Accept connection with new Client, SOCKET CLIENT: %d", connSock);

					// todo flags a changer
					addSocketEpoll(connSock, EPOLLIN | EPOLLOUT);
				}else {
					// client qu'on connait deja
					Logger::log(Logger::DEBUG, "Known client, CLIENT SOCKET: %d", fd);
					handleConnection(fd);
				}
			}
		}
		
	}
	
}
