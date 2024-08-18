#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <queue>
# include <sys/socket.h>
# include <netinet/in.h>

# include "Request.hpp"
# include "Utils.hpp"

# define CLIENT_READ_BUFFER_SIZE 4096

class Client
{
	private:
		int						_fd;
		std::queue<Request>		_requests;

		void	_addRequest(const std::string &rawRequest);
	public:
		Client(void);
		Client(int fd);
		~Client(void);

		/* HANDLE */
		int	handleRequest(void);

		/* GETTERS */
		int getFd(void) const { return _fd; }
};

#endif // CLIENT_HPP
