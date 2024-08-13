#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}

/*
** -------------------------------- METHODS --------------------------------
*/

void Server::cleanSetup(void)
{
	// todo : close les fd socket ouvert
	close(_epollFD);
	for (std::map<std::string, int>::iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it) {
		close(it->second);
	}
}

int Server::check(int ret, std::string msg)
{
	if (ret < 0)
	{
		cleanSetup();
		throw WebservException(Logger::FATAL, msg.c_str());
	}
	return ret;
}

void Server::addSocketEpoll(int sockFD, uint32_t flags)
{
	// Add to epoll
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	check(epoll_ctl(_epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function");
}

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

			check(bind(sockFD, (const sockaddr *)(&addr), sizeof(addr)), "Error with function bind");

			check(fcntl(sockFD, F_SETFL, O_NONBLOCK), "Error with function fcntl");

			// todo changer maxConnexion avec ce qui ya dans le fichier de conf
			int maxconexion = 1000;
			check(listen(sockFD, maxconexion), "Error with function listen");

			_listeningSockets[ipPort] = sockFD;

			addSocketEpoll(sockFD, EPOLLIN);
		}
		else
		{
			// std::cout << "deja la le socket!" << std::endl;
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

void Server::run( void ){

	// todo: a modifier
	const int MAX_EVENTS = 10;
	epoll_event events[MAX_EVENTS];

	while (true)
	{
		Logger::log(Logger::INFO, "Waiting for connections...");
		int nfds = check(epoll_wait(_epollFD, events, MAX_EVENTS, -1), "Error with function epoll wait");
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].events & EPOLLIN){
				int fd = events[i].data.fd;
				// todo a changer 
				if (fd == _listeningSockets[_serversConfig[0].getIpPortJoin()]){
					// nouvelle connexion, faut l'ajouter a epoll
					//todo a checker les parametre
					int connSock = check(accept(fd, NULL, NULL), "Error with accept function");
					check(fcntl(connSock, F_SETFL, O_NONBLOCK), "Error with function fcntl");

					// todo flags a changer
					addSocketEpoll(connSock, EPOLLIN | EPOLLOUT);
				}else {
					handleConnection(fd);
				}
			}
		}
		
	}
	
}
