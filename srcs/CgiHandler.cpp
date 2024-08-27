#include "CgiHandler.hpp"
#include "Webserv.hpp"

CgiHandler::CgiHandler(Response* response, Request* request) : _response(response), _request(request), _env(), _output(), _pid(-1), _envp(NULL), _StdinBackup(-1), _StdoutBackup(-1), _tmpIn(NULL), _tmpOut(NULL), _fdIn(-1), _fdOut(-1), _state(CgiHandler::INIT), _lastActivity(time(NULL))
{
}

CgiHandler::~CgiHandler(void)
{
	if (this->_tmpIn != NULL)
	{
		fclose(this->_tmpIn);
		this->_tmpIn = NULL;
	}
	if (this->_tmpOut != NULL)
	{
		fclose(this->_tmpOut);
		this->_tmpOut = NULL;
	}
	if (this->_fdIn != -1)
	{
		close(this->_fdIn);
		this->_fdIn = -1;
	}
	if (this->_fdOut != -1)
	{
		close(this->_fdOut);
		this->_fdOut = -1;
	}
	if (this->_StdinBackup != -1)
	{
		close(this->_StdinBackup);
		this->_StdinBackup = -1;
	}
	if (this->_StdoutBackup != -1)
	{
		close(this->_StdoutBackup);
		this->_StdoutBackup = -1;
	}
	if (this->_envp)
	{
		for (size_t i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
		this->_envp = NULL;
	}
	if (this->_argv)
	{
		for (size_t i = 0; this->_argv[i]; i++)
			delete[] this->_argv[i];
		delete[] this->_argv;
		this->_argv = NULL;
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
	argv[0] = new char[this->_response->getCgiExecPath().size() + 1];
	strcpy(argv[0], this->_response->getCgiExecPath().c_str());
	argv[1] = new char[this->_response->getCgiPath().size() + 1];
	strcpy(argv[1], this->_response->getCgiPath().c_str());
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
	// std::string path = this->_request->getCgiPath();
	// std::string execPath = this->_request->getCgiExecPath();

	// std::cout << C_RED << "CgiHandler::init" << std::endl;
	// std::cout << "path: " << path << std::endl;
	// std::cout << "execPath: " << execPath << C_RESET << std::endl;

	// Server
	this->_env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->_env["SERVER_NAME"] = this->_request->getHeaders()["Host"];
	this->_env["SERVER_PROTOCOL"] = this->_request->getHttpVersion();
	this->_env["SERVER_PORT"] = this->_request->getClient()->getSocket()->getPort();


	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	//this->_env["SERVER_PORT"] = request->getServer()->getPort();
	this->_env["REQUEST_METHOD"] = this->_request->getMethod();
	this->_env["PATH_INFO"] = this->_response->getCgiPath();
	this->_env["PATH_TRANSLATED"] = this->_response->getCgiPath(); // Not implemented
	this->_env["SCRIPT_NAME"] = this->_response->getCgiExecPath();
	this->_env["SCRIPT_FILENAME"] = this->_response->getCgiExecPath();
	this->_env["QUERY_STRING"] = this->_request->getQuery();
	this->_env["REQUEST_URI"] = this->_request->getUri();

	// this->_env["REMOTE_ADDR"] = this->_request->getClient()->getSocket()->getIp();
	// this->_env["REMOTE_IDENT"] = this->_request->getHeaders()["Authorization"];
	// this->_env["REMOTE_USER"] = this->_request->getHeaders()["Authorization"];

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
		if (dup2(this->_fdIn, STDIN_FILENO) == -1)
			throw ChildProcessException();
		if (dup2(this->_fdOut, STDOUT_FILENO) == -1)
			throw ChildProcessException();
		execve(this->_argv[0], this->_argv, this->_envp);
		throw ChildProcessException();
	}
	else
	{
		this->_state = CgiHandler::PROCESS;
	}
}

/*
** @brief Check the state of the CGI
**
** @return void
*/
void	CgiHandler::checkState(void)
{
	int status;
	pid_t wpid = waitpid(this->_pid, &status, WNOHANG);
	if (wpid == -1) // Error
		throw IntException(500);
	if (wpid == 0) // Still running
		return ;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		// read output
		char	buffer[CGI_READ_BUFFER_SIZE] = {0};

		if (lseek(this->_fdOut, 0, SEEK_SET) == -1)
			throw std::invalid_argument("Error with lseek");
		
		int ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_READ_BUFFER_SIZE);
			ret = read(this->_fdOut, buffer, CGI_READ_BUFFER_SIZE - 1);
			this->_output += buffer;
		}
		this->_parseHeaders();
		this->_state = CgiHandler::FINISH;
	}
	else
	{
		Logger::log(Logger::ERROR, "CGI process exited abnormally");
		throw IntException(500);
	}
}

/*
** @brief Build the response
**
** @return std::string : the response
*/
std::string	CgiHandler::buildResponse(void)
{
	std::string response = "HTTP/1.1 " + intToString(this->_request->getStateCode()) + " " + getErrorMessage(this->_request->getStateCode()) + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n";
	response += this->_output;
	return response;
}



/*
** --------------------------------- PARSING ---------------------------------
*/

/*
** @brief Parse the headers of the CGI
**
** @return void
*/
void	CgiHandler::_parseHeaders(void)
{
	std::cout << C_CYAN << "Output: " << this->_output << C_RESET << std::endl;
	size_t pos = this->_output.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		std::string headers = this->_output.substr(0, pos + 2);
		this->_output= this->_output.substr(pos + 4);
		// trim whitespace output
		// this->_output.erase(remove_if(this->_output.begin(), this->_output.end(), ::isspace), this->_output.end());

		std::istringstream iss(headers);
		std::string line;
		while (std::getline(iss, line, '\n'))
		{
			if (line.empty())
				continue;
			// check if there is a \r at the end of the line
			if (line[line.size() - 1] != '\r')
				throw IntException(502); // TODO: check if it's should be considered as an error
			line.erase(line.size() - 1);
			size_t colonPos = line.find(":");
			if (colonPos == std::string::npos)
				throw IntException(502); // TODO: check if it's should be considered as an error
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
			value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

			if (key == "Status")
			{
				int statusInt = atoi(value.c_str());
				this->_request->setStateCode(statusInt);
				if (statusInt >= 400)
					throw IntException(statusInt);
			}
			else
				this->_headers[key] = value;
		}
		this->_checkHeaders();
	}
	else
	{
		Logger::log(Logger::ERROR, "No headers found in CGI response");
		throw IntException(502); // TODO: check if it's the right error code
	}
}

/*
** --------------------------------- CHECKER ---------------------------------
*/

/*
** @brief Check the headers of the CGI
**
** @return void
*/
void	CgiHandler::_checkHeaders(void)
{
	// printMap(this->_headers);
	// std::cout << C_CYAN << "Checking headers: " << C_RESET << std::endl;
	// for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	// {
	// 	std::cout << C_CYAN << it->first << ": " << it->second << C_RESET << std::endl;
	// }
	// array of string for allowed headers
	// std::string allowedHeaders[] = {"Content-Type", "Content-Length", "Location", "Status"};
	std::vector<std::string> allowedHeaders;
	allowedHeaders.push_back("Content-Type");
	allowedHeaders.push_back("Content-Length");
	allowedHeaders.push_back("Location");
	allowedHeaders.push_back("Status");

	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	{
		if (std::find(allowedHeaders.begin(), allowedHeaders.end(), it->first) == allowedHeaders.end())
			throw IntException(502); // TODO: check if it's the right error code
	}
	// add Content-Length if not present
	if (this->_headers.find("Content-Length") == this->_headers.end())
		this->_headers["Content-Length"] = intToString(this->_output.size());
}