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
# include "Request.hpp"
# include "ConfigParser.hpp"

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
		ConfigParser			_configParser;
		std::map<int, Socket*>	_sockets;
		std::map<int, Client*>	_clients;

		/* SETTERS */
		void setState(int state);
		void setEpollFD(int epollFD) { _epollFD = epollFD; }

		/* UTILS */
		void showIpPortClient(int clientFD);
		void sendResponse(Client* client);
		void handleEvent(epoll_event *events, int i);


		/* HANDLE */
		void	_handleClientConnection(int fd);
		void	_handleClientDisconnection(int fd);

	public:

		Server(void);
		~Server(void);

		void init(void);
		void run(void);
		void stop(void);

		/* GETTERS */
		int getState(void) const { return _state; }
		int getEpollFD(void) const { return _epollFD; }
		ConfigParser& getConfigParser(void) { return _configParser; }
		std::map<int, Socket*> getSockets(void) const { return _sockets; }
		Socket* getSocket(int fd) { return _sockets[fd]; }
		std::map<int, Client*> getClients(void) const { return _clients; }
		Client* getClient(int fd) { return _clients[fd]; }
};


#endif // SERVER_HPP