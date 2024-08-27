#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sys/epoll.h>

#include "Request.hpp"
#include "Client.hpp"
#include "BlocServer.hpp"
#include "Utils.hpp"
#include "BlocLocation.hpp"
#include "ErrorPage.hpp"
#include "CgiHandler.hpp"

class Client;
class Request;
class CgiHandler;

# define RESPONSE_READ_BUFFER_SIZE 4096
# define THRESHOLD_LARGE_FILE 100000 // 100KB

class Cgi
{
	private:
		bool			_alreadyChecked;
		bool			_isCGI;
		std::string		_path;
		std::string		_execPath;
	public:
		Cgi(void) : _alreadyChecked(false), _isCGI(false), _path(""), _execPath("") {}
		~Cgi(void) {}

		// GETTERS
		bool		isAlreadyChecked(void) const { return _alreadyChecked; }
		bool		isCGI(void) const { return _isCGI; }
		std::string	getPath(void) const { return _path; }
		std::string	getExecPath(void) const { return _execPath; }

		// SETTERS
		void	setAlreadyChecked(bool alreadyChecked) { _alreadyChecked = alreadyChecked; }
		void	setIsCGI(bool isCGI) { _isCGI = isCGI; }
		void	setPath(const std::string &path) { _path = path; }
		void	setExecPath(const std::string &execPath) { _execPath = execPath; }
};

class Response
{
	public:
		enum e_response_state
			{
				INIT,
				PROCESS,
				CHUNK,
				FINISH
			};
	
	private:
		Client*				_client;
		Request*			_request;
		Cgi					_cgi;
		CgiHandler*			_cgiHandler;
		std::string 		_response;
		e_response_state	_state;
		int					_fileFd;


		// Methods
		void handleGetRequest();
		bool isRedirect();
		std::vector<std::string> getAllPathsServer();
		void manageServer();
		void manageLocation();
		void manageNotFound(std::string directoryToCheck);
		std::string findGoodPath(std::vector<std::string> allPaths);
		bool isLargeFile(const std::string& path);
		void prepareStandardResponse(const std::string &path);
		void prepareChunkedResponse(const std::string &path);

		// Setters
		void setState(e_response_state state);
		void setHeaderChunked(const std::string &path);

		// Check
		bool	_checkCgiPath(std::string path);


	public:
		// Response();
		Response(Client* client);
		~Response();

		
		// Getters
		int getState() const { return _state; }
		std::string getResponse() const { return _response; }
		size_t	getResponseSize() const { return _response.size(); }
		void generateResponse(int epollFD);
		std::vector<std::string> getAllPathsLocation();
		CgiHandler* getCgiHandler() const { return _cgiHandler; }

		// Setters
		void setError(int code);

		// Check
		void	checkCgi(void);
		void  clearCgi();

		// Handle
		int	handleCGI(int epollFD);

		// CGI
		bool isCGI(void) const { return _cgi.isCGI(); }
		std::string getCgiPath(void) const { return _cgi.getPath(); }
		std::string getCgiExecPath(void) const { return _cgi.getExecPath(); }
		void	setCgi(bool isCgi, const std::string &path, const std::string &execPath) { _cgi.setIsCGI(isCgi); _cgi.setPath(path); _cgi.setExecPath(execPath); }

};

#endif /* ******************************************************** RESPONSE_H */