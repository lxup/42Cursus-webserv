#include "CgiExecutor.hpp"

CgiExecutor::CgiExecutor(RequestCgi *requestCgi) : _requestCgi(requestCgi), _envp(NULL), _argv(NULL), _body(true), _pid(-1), _StdinBackup(-1), _StdoutBackup(-1)
{
}

CgiExecutor::CgiExecutor(const CgiExecutor &src)
{
	*this = src;
}

CgiExecutor::~CgiExecutor(void)
{
	if (this->_envp)
	{
		for (int i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
	}
	if (this->_argv)
	{
		for (int i = 0; this->_argv[i]; i++)
			delete[] this->_argv[i];
		delete[] this->_argv;
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
}

CgiExecutor &CgiExecutor::operator=(const CgiExecutor &src)
{
	if (this != &src)
	{
		this->_requestCgi = src._requestCgi;
	}
	return *this;
}

void CgiExecutor::_init(void)
{
	Logger::log(Logger::DEBUG, "[CgiExecutor::_init] Start CGI Handler V2");
	Logger::log(Logger::DEBUG, "[CgiExecutor::_init] Path: %s", this->_requestCgi->_path.c_str());
	Logger::log(Logger::DEBUG, "[CgiExecutor::_init] ExecPath: %s", this->_requestCgi->_execPath.c_str());

	// Add env variables
	this->_env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->_env["SERVER_NAME"] = this->_requestCgi->_request->_headers["Host"];
	this->_env["SERVER_PROTOCOL"] = this->_requestCgi->_request->_httpVersion;
	this->_env["SERVER_PORT"] = this->_requestCgi->_request->_client->getSocket()->getPort();

	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["REQUEST_METHOD"] = this->_requestCgi->_request->_method;
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = this->_requestCgi->_path;
	this->_env["SCRIPT_FILENAME"] = this->_requestCgi->_path;
	this->_env["PATH_INFO"] = this->_requestCgi->_path;
	this->_env["PATH_TRANSLATED"] = this->_requestCgi->_path; // Not implemented
	this->_env["QUERY_STRING"] = this->_requestCgi->_request->_query;
	this->_env["REQUEST_URI"] = this->_requestCgi->_request->_uri;
	this->_env["REMOTE_ADDR"] = this->_requestCgi->_request->_client->getSocket()->getIp();
	this->_env["REMOTE_IDENT"] = this->_requestCgi->_request->_headers["Authorization"];
	this->_env["REMOTE_USER"] = this->_requestCgi->_request->_headers["Authorization"];
	this->_env["CONTENT_LENGTH"] = Utils::ullToStr(this->_requestCgi->_request->_body._size);
	this->_env["CONTENT_TYPE"] = this->_requestCgi->_request->_headers["Content-Type"];
	this->_env["HTTP_COOKIE"] = this->_requestCgi->_request->_headers["Cookie"];
}

void CgiExecutor::_execute(void)
{
	Logger::log(Logger::DEBUG, "[CgiExecutor::_execute] Start CGI Handler V2");
	this->_envp = this->_envToChar();
	// print envp
	// for (size_t i = 0; this->_envp[i]; i++)
	// 	std::cout << "envp[" << i << "]: " << this->_envp[i] << std::endl;
	this->_argv = this->_buildArgv();
	// TODO: maybe check if envp and argv are not NULL

	this->_StdinBackup = dup(STDIN_FILENO);
	this->_StdoutBackup = dup(STDOUT_FILENO);

	// Build the output body
	// this->_tmpOut = tmpfile();
	// if (this->_tmpOut == NULL)
	// 	throw std::invalid_argument("[CgiExecutor::_execute] tmpfile failed");
	// this->_fdOut = fileno(this->_tmpOut);
	// if (this->_fdOut == -1)
	// 	throw std::invalid_argument("[CgiExecutor::_execute] fileno failed");
	if (Utils::createTmpFile(this->_body._path, this->_body._fd) == -1)
		throw std::invalid_argument("[CgiExecutor::_execute] createTmpFile failed");

	// if there is a body, we need to put the body in the stdin
	if (this->_requestCgi->_request->_body._fd != -1) // Start from the beginning of the body, if there is one
		if (lseek(this->_requestCgi->_request->_body._fd, 0, SEEK_SET) == -1)
			throw std::invalid_argument("[CgiExecutor::_execute] lseek failed");

	// // SHOW BODY
	// std::cout << "SHOOOOOOOOOWWWWWWWW BODY" << std::endl;
	// int bufferSize = 100;
	// char	buffer[bufferSize] = {0};
	// int ret = 1;
	// while (ret > 0)
	// {
	// 	memset(buffer, 0, bufferSize);
	// 	ret = read(this->_requestCgi->_request->_body._fd, buffer, bufferSize - 1);
	// 	if (ret == -1)
	// 		throw std::invalid_argument("[CgiExecutor::_execute] read failed");
	// 	std::string bufferStr(buffer, ret);
	// 	std::cout << "buffer: " << bufferStr << std::endl;
	// }
	// if (lseek(this->_requestCgi->_request->_body._fd, 0, SEEK_SET) == -1)
	// 	throw std::invalid_argument("[CgiExecutor::_execute] lseek failed");
	// // END SHOW BODY
	
	this->_pid = fork();
	if (this->_pid == -1)
		throw std::invalid_argument("[CgiExecutor::_execute] fork failed");
	if (this->_pid == 0)
	{
		if (this->_requestCgi->_request->_body._fd != -1) // Listen to the body, if there is one
			if (dup2(this->_requestCgi->_request->_body._fd, STDIN_FILENO) == -1)
				throw ChildProcessException();
		if (dup2(this->_body._fd, STDOUT_FILENO) == -1)
			throw ChildProcessException();
		execve(this->_argv[0], this->_argv, this->_envp);
		throw ChildProcessException();
	}
	else
	{
		this->_requestCgi->_request->_setState(Request::CGI_PROCESS);
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
char	**CgiExecutor::_envToChar(void)
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
char	**CgiExecutor::_buildArgv(void)
{
	char	**argv = new char*[3];
	argv[0] = new char[this->_requestCgi->_execPath.size() + 1];
	strcpy(argv[0], this->_requestCgi->_execPath.c_str());
	argv[1] = new char[this->_requestCgi->_path.size() + 1];
	strcpy(argv[1], this->_requestCgi->_path.c_str());
	argv[2] = NULL;
	return argv;
}
