#include "Socket.hpp"

Socket::Socket(void) : _fd(-1)
{
}

Socket::Socket(int fd, std::string ip, unsigned int port, std::vector<BlocServer>* servers) : _fd(fd), _ip(ip), _port(port), _servers(servers)
{
	Logger::log(Logger::INFO, "Initializing socket on %s:%d", ip.c_str(), port);
	try {
		this->_addr.sin_family = AF_INET;
		this->_addr.sin_port = htons(port);
		this->_addr.sin_addr.s_addr = inet_addr(ip.c_str());
		protectedCall(fcntl(this->_fd, F_SETFL, O_NONBLOCK), "fcntl");
		int optval = 1;
		protectedCall(setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)), "setsockopt");
		protectedCall(bind(this->_fd, (struct sockaddr *)&this->_addr, sizeof(this->_addr)), "bind");
		protectedCall(listen(this->_fd, BACKLOGS), "listen");
	}
	catch (std::exception &e) {
		if (this->_fd != -1)
			protectedCall(close(this->_fd), "[Socket] Faild to close socket", false);
		Logger::log(Logger::FATAL, "Failed to initialize socket on %s:%d", ip.c_str(), port);
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
