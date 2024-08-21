#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <queue>
# include <sys/socket.h>
# include <netinet/in.h>

# include "Utils.hpp"
# include "Request.hpp"
# include "Socket.hpp"

# define CLIENT_READ_BUFFER_SIZE 10 // 4096

class Request;

class Client
{
	private:
		int						_fd;
		Socket*					_socket;
		Request*				_request;
	public:
		Client(void);
		Client(int fd, Socket* socket);
		~Client(void);

		/* HANDLE */
		int	handleRequest(void);
		void clearRequest(void);

		/* GETTERS */
		int getFd(void) const { return _fd; }
		Request* getRequest(void) const { return _request; }
		Socket* getSocket(void) const { return _socket; }
};

#endif // CLIENT_HPP
