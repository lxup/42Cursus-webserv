#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <fstream>

# include "Request.hpp"
# include "BlocServer.hpp"
# include "Utils.hpp"

class Request;

class Response
{
	public:
		Response(void);
		Response(Request* request, BlocServer blocServer);
		~Response();

		std::string getRawResponse( void );

	private:
		Request*	_request;
		BlocServer _blocServer;

		std::string _response;
		
		// Methods
		void handleGetRequest(void);
		// std::vector<std::string> getPathRessource(void);



};

#endif /* ******************************************************** RESPONSE_H */