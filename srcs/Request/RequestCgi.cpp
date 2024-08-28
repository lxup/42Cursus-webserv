#include "RequestCgi.hpp"

RequestCgi::RequestCgi(void) : _request(NULL), _isCGI(false), _path(""), _execPath(""), _cgiHandler(NULL) {}

RequestCgi::RequestCgi(Request* request) : _request(request), _isCGI(false), _path(""), _execPath(""), _cgiHandler(NULL) {}

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
		this->_cgiHandler = new CgiExecutor(this);
		if (this->_cgiHandler == NULL)
			throw std::bad_alloc();
		this->_cgiHandler->_init();
		this->_cgiHandler->_execute();
	} catch (ChildProcessException &e) {
		throw ChildProcessException();
	} catch (IntException &e) {
		return (this->_request->setError(e.code()));
	} catch (std::exception &e) {
		Logger::log(Logger::ERROR, "Failed to handle CGI: %s", e.what());
		return (this->_request->setError(500));
	}
}

/*
** @brief Check if the CGI is finished
**
** @return bool : true if the CGI is finished, false otherwise
*/
void	RequestCgi::_checkState(void)
{
	if (this->_request->_state == Request::FINISH)
		return ;
	if (this->_cgiHandler == NULL)
		return ;
	try {
		int status;
		pid_t wpid = waitpid(this->_cgiHandler->_pid, &status, WNOHANG);
		if (wpid == -1) // Error
			throw IntException(500);
		if (wpid == 0) // Still running
			return ;
		// if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		// {
		this->_request->_setState(Request::FINISH);
		// }
		// else
		// {
		// 	Logger::log(Logger::ERROR, "[RequestCgi] CGI process exited abnormally");
		// 	throw IntException(500);
		// }
	} catch (IntException &e) {
		Logger::log(Logger::DEBUG, "[RequestCgi] CGI process failed: %d", e.code());
		return (this->_request->setError(e.code()));
	}
}

/*
** @brief Kill the CGI process
*/
void	RequestCgi::_kill(void)
{
	if (this->_cgiHandler == NULL)
		return ;
	kill(this->_cgiHandler->_pid, SIGTERM); // OR SIGKILL
}