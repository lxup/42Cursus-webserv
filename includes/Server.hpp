#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserv.hpp"

class Server
{
	private:
		bool _isRunning;
		int _epollFD;

		std::vector<BlocServer> _serversConfig; // all the Configuration for each server
		std::map<std::string, int> _listeningSockets; // ip:port map to the server socket
		
		std::map<int, std::queue<std::string> > _clientRequests; // TODO : format Reponse instead of string

		int check(int ret, std::string msg);
		void addSocketEpoll(int sockFD, uint32_t flags);
		void modifySocketEpoll(int sockFD, uint32_t flags);
		void deleteSocketEpoll(int sockFD);
		void showIpPortClient(int clientFD);
		void handleConnection(int clientFD);
		void handleEvent(int fd, uint32_t event);
		bool isNewConnection(int fd);
		void closeConnection(int fd);
		void sendResponse(int fd);


	public:

		Server();
		~Server();

		void init(std::vector<BlocServer> serversConfig);
		void run( void );
		void stop( void );
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */