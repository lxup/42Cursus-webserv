#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>

#include "Request.hpp"
#include "BlocServer.hpp"
#include "Utils.hpp"
#include "BlocLocation.hpp"
#include "ErrorPage.hpp"

class Request;

# define RESPONSE_READ_BUFFER_SIZE 1000
//# define THRESHOLD_LARGE_FILE 1000000 // 1MB
# define THRESHOLD_LARGE_FILE 1000 // 1KB

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
		const Request *_request;
		const BlocServer *_blocServer;
		const BlocLocation* _blocLocation;
		std::string _response;
		e_response_state _state;	

		int _fileFd;

		// Methods
		void handleGetRequest();
		bool isRedirect();
		std::vector<std::string> getAllPathsServer();
		std::vector<std::string> getAllPathsLocation();
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


	public:
		Response();
		Response(Request *request);
		~Response();

		
		// Getters
		int getState() const { return _state; }
		std::string getRawResponse();

};

#endif /* ******************************************************** RESPONSE_H */