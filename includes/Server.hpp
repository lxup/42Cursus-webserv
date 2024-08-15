#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserv.hpp"

class Server
{
	private:
		bool _isRunning;
		int _epollFD;
		std::vector<BlocServer> _serversConfig;
		// ip:port map to the server socket
		std::map<std::string, int> _listeningSockets;

		int check(int ret, std::string msg);
		void addSocketEpoll(int sockFD, uint32_t flags);
		
		
		void showIpPortClient(int clientFD);
		void handleConnection(int clientFD);

		bool isNewConnection(int fd);






	public:

		Server();
		~Server();

		void init(std::vector<BlocServer> serversConfig);
		void run( void );
		void stop( void );
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */