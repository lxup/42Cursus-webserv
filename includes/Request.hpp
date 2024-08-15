#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <string>
# include <Webserv.hpp>
#include "BlocLocation.hpp"


class Request
{
	private:
		std::vector<e_Methods> _method;
		std::string _protocols;
		std::string _host;
		std::string _referer;
		unsigned int _nbChar;
		
	public:
		Request();
		~Request();
		void storageValue(char *str, unsigned int bytes);
};
#endif
