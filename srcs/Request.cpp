#include "Request.hpp"

Request::Request() : _method(GET), _protocols(""), _host(""), _referer(""), _nbChar(0)
{
}

Request::~Request()
{
}

void Request::addAllowedMethods(std::string &token)
{
	e_Methods met;

	if (token == "GET")
		met = GET;
	else if (token == "POST")
		met = POST;
	else if (token == "DELETE")
		met = DELETE;
	else
		met = BAD_MET;
	_method = met;
}

void Request::storageValue(char *cha, unsigned int bytes)
{
	std::string str = cha;
	_nbChar = bytes;
	std::vector<std::string> ve = split(cha, " ");

	this->addAllowedMethods(ve[0]);
	this->_protocols = ve[2];
	std::vector<std::string>::iterator it = std::find(ve.begin(), ve.end(), "Host:");
	if (!(*it).empty())
	{
		it++;
		_host = *it;
	}
}

void Request::printValue()
{
	std::cout << _method << std::endl;
	std::cout << _protocols << std::endl;
	std::cout << _host << std::endl;
	std::cout << _nbChar << std::endl;
}
