#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserv.hpp"

class Server
{
	private:
		int _epollFD;
		std::vector<BlocServer> _serversConfig;
		std::map<std::string, int> _listeningSockets;


		int check(int ret, std::string msg);
		void cleanSetup( void );
		void addSocketEpoll(int sockFD, uint32_t flags);
		
		
		void showIpClient(int clientFD);
		void handleConnection(int clientFD);






	public:

		Server();
		~Server();

		void init(std::vector<BlocServer> serversConfig);
		void run( void );
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */