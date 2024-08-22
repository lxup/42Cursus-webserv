#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>

#include "Request.hpp"
#include "BlocServer.hpp"
#include "Utils.hpp"
#include "BlocLocation.hpp"
#include "ErrorPage.hpp"

class Request;

class Response
{
	public:
		enum e_response_state
			{
				INIT,
				PROCESS,
				FINISH
			};
	private:
		const Request *_request;
		const BlocServer *_blocServer;
		const BlocLocation* _blocLocation;

		std::string _response;

		e_response_state _state;	

		// Methods
		void handleGetRequest();
		bool isRedirect();
		std::vector<std::string> getAllPathsServer();
		std::vector<std::string> getAllPathsLocation();
		void initBlocLocation();
		void manageServer();
		void manageLocation();


		// Setters
		void setState(e_response_state state);
	public:
		Response();
		Response(Request *request);
		~Response();

		std::string getRawResponse();

};

#endif /* ******************************************************** RESPONSE_H */