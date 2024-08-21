#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <fstream>

# include "Request.hpp"
# include "BlocServer.hpp"
# include "Utils.hpp"

class Response
{


	public:

		Response(Request request, BlocServer blocServer);
		~Response();

		std::string getResponse( void );

	private:
		Request _request;
		BlocServer _blocServer;

};

#endif /* ******************************************************** RESPONSE_H */