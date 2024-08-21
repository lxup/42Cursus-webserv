#include "Socket.hpp"

Socket::Socket(void) : _fd(-1)
{
}

Socket::Socket(std::string ip, unsigned int port, std::vector<BlocServer>* servers)
{
	Logger::log(Logger::DEBUG, "Initializing socket on %s:%d", ip.c_str(), port);
	this->_ip = ip;
	this->_port = port;
	this->_fd = protectedCall(socket(AF_INET, SOCK_STREAM, 0), "socket");
	this->_servers = servers;
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_port = htons(port);
	this->_addr.sin_addr.s_addr = inet_addr(ip.c_str());

	try {
		protectedCall(fcntl(this->_fd, F_SETFL, O_NONBLOCK), "fcntl");
		int optval = 1;
		protectedCall(setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)), "setsockopt");
		protectedCall(bind(this->_fd, (struct sockaddr *)&this->_addr, sizeof(this->_addr)), "bind");
		protectedCall(listen(this->_fd, BACKLOGS), "listen");
	}
	catch (std::exception &e) {
		protectedCall(close(this->_fd), "close", false);
		throw std::exception();
	}

}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket::~Socket(void)
{
	if (this->_fd != -1)
		protectedCall(close(this->_fd), "Faild to close socket", false);
}

Socket &Socket::operator=(const Socket &rhs)
{
	if (this != &rhs)
	{
		this->_ip = rhs._ip;
		this->_port = rhs._port;
		this->_fd = rhs._fd;
		this->_servers = rhs._servers;
		this->_addr = rhs._addr;
	}
	return *this;
}
