#include "CgiHandler.hpp"

CgiHandler::CgiHandler(void)
{
}

CgiHandler::~CgiHandler(void)
{
}


/*
** --------------------------------- UTILS ---------------------------------
*/

/*
** @brief Initialize the CgiHandler
**
** @param request
** @return int : 0 on success, -1 on error
*/
int		CgiHandler::init(Request* request)
{
	_env["SERVER_SOFTWARE"] = "webserv/1.0";
	_env["SERVER_NAME"] = request->getHeaders()["Host"];
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = request->getHttpVersion();
	//_env["SERVER_PORT"] = request->getServer()->getPort();
	_env["REQUEST_METHOD"] = request->getMethod();
	_env["PATH_INFO"] = request->getPath();
	_env["PATH_TRANSLATED"] = request->getPath(); // Not implemented
	_env["SCRIPT_NAME"] = this->_execPath; // TODO
	// _env["QUERY_STRING"] = request->getQueryString();
	
	return 0;
}

/*
** @brief Reset the CgiHandler
**
** @return void
*/
void	CgiHandler::reset(void)
{
	_env.clear();
	_pid = 0;
	_pipe[0] = 0;
	_pipe[1] = 0;
	_exitStatus = 0;
}
