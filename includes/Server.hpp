#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include <vector>
# include <sys/epoll.h>
# include <arpa/inet.h>
# include <algorithm>

# include "Socket.hpp"
# include "Client.hpp"
# include "BlocServer.hpp"
# include "Response.hpp"
# include "Utils.hpp"

#include "Request.hpp"

// define step for server 
enum ServerState
{
	S_STATE_INIT = 0,
	S_STATE_READY,
	S_STATE_RUN,
	S_STATE_STOP
};

class Server
{
	private:
		int						_state;
		int 					_epollFD;
		std::map<int, Socket*>	_sockets;
		std::map<int, Client*>	_clients;

		/* SETTERS */
		void setState(int state);
		void setEpollFD(int epollFD) { _epollFD = epollFD; }


		// uint32_t 	_currentFlagEpoll;

		// std::vector<BlocServer> 				_serversConfig; // all the Configuration for each server
		// std::map<std::string, int>				_listeningSockets; // ip:port map to the server socket
		
		// std::map<int, std::queue<std::string> > _clientRequests; // TODO : format Reponse instead of string

		int check(int ret, std::string msg);
		void showIpPortClient(int clientFD);
		void handleConnection(int clientFD);
		void handleEvent(int fd, uint32_t event);
		bool isNewConnection(int fd);
		void closeConnection(int fd);
		void sendResponse(Client &client);

		/* HANDLE */
		void	_handleClientConnection(int fd);
		void	_handleClientDisconnection(int fd);

	public:

		Server(void);
		~Server(void);

		void init(std::map<std::string, std::vector<BlocServer> > servers);
		void run(void);
		void stop(void);

		/* GETTERS */
		int getState(void) const { return _state; }
		int getEpollFD(void) const { return _epollFD; }
		std::map<int, Socket*> getSockets(void) const { return _sockets; }
		Socket* getSocket(int fd) { return _sockets[fd]; }
		std::map<int, Client*> getClients(void) const { return _clients; }
		Client* getClient(int fd) { return _clients[fd]; }

		/* CLIENTS */
		void addClient(int fd, Client* client);
		void deleteClient(int fd);
};

void printEvent(int fd, uint32_t event);

#endif // SERVER_HPP