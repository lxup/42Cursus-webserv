#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>

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
		Request*			_request;
		CgiHandler*			_cgi;
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
		std::string getRawResponse();
		std::vector<std::string> getAllPathsLocation();

		// Setters
		void setError(int code);

		// Check
		int	checkCgi(void);

		// Handle
		int	handleCGI(void);
		

};

#endif /* ******************************************************** RESPONSE_H */