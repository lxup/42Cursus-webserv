#include "RequestCgi.hpp"

RequestCgi::RequestCgi(void) : _request(NULL), _isCGI(false), _path(""), _execPath("") {}

RequestCgi::RequestCgi(Request* request) : _request(request), _isCGI(false), _path(""), _execPath("") {}

RequestCgi::RequestCgi(const RequestCgi &src)
{
	*this = src;
}

RequestCgi::~RequestCgi(void)
{
	if (this->_cgiHandler)
		delete this->_cgiHandler;
}

RequestCgi &RequestCgi::operator=(const RequestCgi &src)
{
	if (this != &src)
	{
		this->_request = src._request;
		this->_isCGI = src._isCGI;
		this->_path = src._path;
		this->_execPath = src._execPath;
		this->_cgiHandler = src._cgiHandler;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** @brief Initialize the CgiHandler
*/
void	RequestCgi::_start(void)
{
	try {
		if (this->_cgiHandler)
			delete this->_cgiHandler;
		this->_cgiHandler = new CgiHandlerV2(this);
		if (this->_cgiHandler == NULL)
			throw std::bad_alloc();
		this->_cgiHandler->_init();
		this->_cgiHandler->_execute();
		// modifySocketEpoll(this->_request->_client)
	} catch (ChildProcessException &e) {
		throw ChildProcessException();
	} catch (IntException &e) {
		return (this->_request->setError(e.code()));
	} catch (std::exception &e) {
		Logger::log(Logger::ERROR, "Failed to handle CGI: %s", e.what());
		return (this->_request->setError(500));
	}
}