#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <ctime>

# include "Utils.hpp"
# include "Request.hpp"
# include "Socket.hpp"
# include "Response.hpp"

# define CLIENT_READ_BUFFER_SIZE 1 // 4096

class Request;
class Response;


class Client
{
	private:
		int						_fd;
		Socket*					_socket;
		Request*				_request;
		Response*				_response;
		time_t					_lastActivity;

	public:
		// Client(void);
		Client(int fd, Socket* socket);
		~Client(void);

		/* HANDLE */
		void	handleRequest( int epollFD );
		void handleResponse(int epollFD);
		
		void reset(void);

		/* GETTERS */
		int getFd(void) const { return _fd; }
		Request* getRequest(void) const { return _request; }
		Socket* getSocket(void) const { return _socket; }
		Response* getResponse(void) const { return _response; }

		// timeout
		time_t getLastActivity() const { return _lastActivity; }
		void updateLastActivity() { _lastActivity = time(NULL); }

		// Is
		bool	isCgiReady(int epollFD);

};

class ClientDisconnectedException : public std::exception
{
	public:
		virtual const char* what() const throw()
		{
			return "Client disconnected";
		}
};


#endif // CLIENT_HPP
