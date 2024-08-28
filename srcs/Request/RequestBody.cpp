#include "RequestBody.hpp"

RequestBody::RequestBody(void) : _request(NULL), _fd(-1), _isTmp(false), _size(0)
{
}

RequestBody::RequestBody(Request *request) : _request(request), _fd(-1), _isTmp(false), _size(0)
{

}

RequestBody::RequestBody(const RequestBody &src)
{
	*this = src;
}

RequestBody::~RequestBody(void)
{
	if (this->_fd != -1)
		protectedCall(close(this->_fd), "failed to close file", false);
	if (this->_path.size() && this->_isTmp)
		protectedCall(remove(this->_path.c_str()), "failed to remove file", false);
}

RequestBody &RequestBody::operator=(const RequestBody &rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_fd = rhs._fd;
		// this->_stream = rhs._stream;
		this->_isTmp = rhs._isTmp;
		this->_size = rhs._size;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** @brief Write data in the file
**
** @param data : The data to write
*/
int	RequestBody::_write(const std::string &data)
{
	if (this->_fd == -1)
	{
		Logger::log(Logger::ERROR, "[_write] File descriptor not set");
		return (-1);
	}
	if (write(this->_fd, data.c_str(), data.size()) == -1)
	{
		Logger::log(Logger::ERROR, "[_write] Error writing in file");
		return (-1);
	}
	this->_size += data.size();
	return (0);
}
