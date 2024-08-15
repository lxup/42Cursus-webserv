#include "Request.hpp"

Request::Request() : _protocols(""), _host(""), _referer(""), _nbChar(0)
{
	_method.push_back(GET);
	std::cout << "Request default constructor called\n";
}

Request::~Request()
{
	std::cout << "Destructor of Request called\n";
}
