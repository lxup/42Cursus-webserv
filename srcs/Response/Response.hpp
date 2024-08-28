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

# define RESPONSE_READ_BUFFER_SIZE 4096
# define THRESHOLD_LARGE_FILE 100000 // 100KB

class Response
{
	friend class CgiHandler;
	public:
		enum e_response_state
			{
				INIT,
				PROCESS,
				CHUNK,
				FINISH
			};
	
	private:
		//Client*				_client;
		Request*			_request;
		CgiHandler			_cgiHandler;
		std::string 		_response;
		e_response_state	_state;
		int					_fileFd;


		// Methods
		void handleGetRequest();
		void handlePostRequest();
		void handlePutRequest();
		void handleDeleteRequest();

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

		/* HANDLE */
		int	_handleCgi(void);

	public:
		// Response();
		Response(Client* client);
		~Response();

		
		// Getters
		int getState() const { return _state; }
		std::string getResponse() const { return _response; }
		size_t	getResponseSize() const { return _response.size(); }
		int generateResponse(int epollFD);
		std::vector<std::string> getAllPathsLocation();
		CgiHandler &getCgiHandler(void) { return _cgiHandler; }

		// Setters
		void setError(int code, bool generatePage = true);
};

#endif /* ******************************************************** RESPONSE_H */