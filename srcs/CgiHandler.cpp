#include "CgiHandler.hpp"
#include "Webserv.hpp"

CgiHandler::CgiHandler(Request* request) : _request(request), _env(), _body(), _pid(-1), _envp(NULL), _StdinBackup(-1), _StdoutBackup(-1), _tmpIn(NULL), _tmpOut(NULL), _fdIn(-1), _fdOut(-1)
{
}

CgiHandler::~CgiHandler(void)
{
	if (this->_tmpIn != NULL)
		fclose(this->_tmpIn);
	if (this->_tmpOut != NULL)
		fclose(this->_tmpOut);
	if (this->_fdIn != -1)
		close(this->_fdIn);
	if (this->_fdOut != -1)
		close(this->_fdOut);
	if (this->_StdinBackup != -1)
		close(this->_StdinBackup);
	if (this->_StdoutBackup != -1)
		close(this->_StdoutBackup);
	if (this->_envp)
	{
		for (size_t i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
	}
	if (this->_argv)
	{
		for (size_t i = 0; this->_argv[i]; i++)
			delete[] this->_argv[i];
		delete[] this->_argv;
	}
}


/*
** --------------------------------- UTILS ---------------------------------
*/

/*
** @brief Convert the _env map to a char**
**
** @return char** : the char** of the _env map
*/
char	**CgiHandler::_envToChar(void)
{
	char	**envp = new char*[_env.size() + 1];
	std::map<std::string, std::string>::const_iterator it;
	int i = 0;

	for (it = _env.begin(); it != _env.end(); ++it){
		std::string env = it->first + "=" + it->second;
		envp[i] = new char[env.size() + 1];
		strcpy(envp[i], env.c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

/*
** @brief Build the argv
**
** @return char** : the argv
*/
char	**CgiHandler::_buildArgv(void)
{
	char	**argv = new char*[3];
	argv[0] = new char[this->_request->getCgiExecPath().size() + 1];
	strcpy(argv[0], this->_request->getCgiExecPath().c_str());
	argv[1] = new char[this->_request->getCgiPath().size() + 1];
	strcpy(argv[1], this->_request->getCgiPath().c_str());
	argv[2] = NULL;
	return argv;
}


/*
** ---------------------------------- MAIN ---------------------------------
*/

/*
** @brief Initialize the CgiHandler
**
** @param request
** @return int : 0 on success, -1 on error
*/
void		CgiHandler::init(void)
{
	std::string path = this->_request->getCgiPath();
	std::string execPath = this->_request->getCgiExecPath();

	std::cout << C_RED << "CgiHandler::init" << std::endl;
	std::cout << "path: " << path << std::endl;
	std::cout << "execPath: " << execPath << C_RESET << std::endl;

	// Server
	this->_env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->_env["SERVER_NAME"] = this->_request->getHeaders()["Host"];
	this->_env["SERVER_PROTOCOL"] = this->_request->getHttpVersion();
	this->_env["SERVER_PORT"] = this->_request->getClient()->getSocket()->getPort();


	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	//this->_env["SERVER_PORT"] = request->getServer()->getPort();
	this->_env["REQUEST_METHOD"] = this->_request->getMethod();
	this->_env["PATH_INFO"] = this->_request->getCgiPath();
	this->_env["PATH_TRANSLATED"] = this->_request->getCgiPath(); // Not implemented
	this->_env["SCRIPT_NAME"] = this->_request->getCgiExecPath();
	this->_env["SCRIPT_FILENAME"] = this->_request->getCgiExecPath();
	this->_env["QUERY_STRING"] = this->_request->getQuery();

	this->_env["CONTENT_LENGTH"] = intToString(this->_request->getBodySize());
	this->_env["CONTENT_TYPE"] = this->_request->getHeaders()["Content-Type"];
	// std::cout << "SERVER_NAME: " << _env["SERVER_NAME"] << std::endl;

	// exit(0);
}

/*
** @brief Execute the CgiHandler
**
** @return void
*/
void	CgiHandler::execute(void)
{
	try {
		this->_envp = this->_envToChar();
	} catch (std::bad_alloc &e) {
		Logger::log(Logger::ERROR, "Error with envToChar: %s", e.what());
	}
	try {
		this->_argv = this->_buildArgv();
	} catch (std::bad_alloc &e) {
		Logger::log(Logger::ERROR, "Error with buildArgv: %s", e.what());
	}
	if (this->_envp == NULL || this->_argv == NULL)
		throw std::invalid_argument("Error with envToChar or buildArgv");
	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
		throw std::invalid_argument("Error with buildArgv");
	this->_StdinBackup = dup(STDIN_FILENO);
	this->_StdoutBackup = dup(STDOUT_FILENO);

	this->_tmpIn = tmpfile();
	this->_tmpOut = tmpfile();
	this->_fdIn = fileno(this->_tmpIn);
	this->_fdOut = fileno(this->_tmpOut);
	if (this->_fdIn == -1 || this->_fdOut == -1)
		throw std::invalid_argument("Error with tmpfile");
	
	if (write(this->_fdIn, this->_request->getBody().c_str(), this->_request->getBodySize()) == -1)
		throw std::invalid_argument("Error with write");
	if (lseek(this->_fdIn, 0, SEEK_SET) == -1)
		throw std::invalid_argument("Error with lseek");

	this->_pid = fork();
	if (this->_pid == -1)
		throw std::invalid_argument("Error with fork");
	if (this->_pid == 0)
	{
		try {
			if (dup2(this->_fdIn, STDIN_FILENO) == -1)
				throw std::exception();
			if (dup2(this->_fdOut, STDOUT_FILENO) == -1)
				throw std::exception();
			execve(this->_argv[0], this->_argv, this->_envp);
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		}	catch (std::exception &e) {
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		}
	}
	else
	{
		char	buffer[CGI_READ_BUFFER_SIZE] = {0};


		waitpid(-1, NULL, 0);
		if (lseek(this->_fdOut, 0, SEEK_SET) == -1)
			throw std::invalid_argument("Error with lseek");

		int ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_READ_BUFFER_SIZE);
			ret = read(this->_fdOut, buffer, CGI_READ_BUFFER_SIZE - 1);
			this->_body += buffer;
		}
	}

	if (dup2(this->_StdinBackup, STDIN_FILENO) == -1)
		throw std::invalid_argument("Error with dup2");
	if (dup2(this->_StdoutBackup, STDOUT_FILENO) == -1)
		throw std::invalid_argument("Error with dup2");
	this->_handleExitStatus();
	return ;
}

/*
** @brief Set the headers of the response
**
** @return void
*/
void	CgiHandler::setHeaders(void)
{
	std::string header;
	std::string body;

	size_t pos = this->_body.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		header = this->_body.substr(0, pos);
		body = this->_body.substr(pos + 4);
	}
	else
	{
		header = this->_body;
		body = "";
	}

	this->_bodyWithHeaders = "HTTP/1.1 200 OK\r\n";
	this->_bodyWithHeaders += header + "\r\n";
	this->_bodyWithHeaders += "Content-Length: " + intToString(body.size()) + "\r\n";
	this->_bodyWithHeaders += "\r\n";
	this->_bodyWithHeaders += body;
}

/*
** --------------------------------- HANDLE ---------------------------------
*/

/*
** @brief Handle the exit status of the CGI
**
** @param status
** @return void
*/
void	CgiHandler::_handleExitStatus(void)
{
	// TODO : MAKE A BETTER HANDLING
	size_t pos = this->_body.find("Status: ");
	if (pos != std::string::npos)
	{
		std::string status = this->_body.substr(pos + 8, 3);
		this->_body = this->_body.substr(0, pos) + this->_body.substr(pos + 11);

		int statusInt = atoi(status.c_str());
		this->_request->setStateCode(statusInt);
		if (statusInt != REQUEST_DEFAULT_STATE_CODE)
			throw IntException(statusInt);
	}

}
