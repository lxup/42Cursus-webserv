#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <string>
# include <Webserv.hpp>
#include "BlocLocation.hpp"


class Request
{
	private:
		e_Methods _method;
		std::string _protocols;
		std::string _host;
		std::string _referer;
		unsigned int _nbChar;
		
	public:
		Request();
		~Request();
		void storageValue(char *str, unsigned int bytes);
		void addAllowedMethods(std::string &token);
		void printValue();

};
#endif
