#include "Request.hpp"

Request::Request() : _protocols(""), _host(""), _referer(""), _nbChar(0)
{
	_method.push_back(GET);
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
	if (std::find(_method.begin(), _method.end(), met) != _method.end())
		return ;
	_method.push_back(met);
}

void Request::storageValue(char *cha, unsigned int bytes)
{
	std::string str = cha;
	_nbChar = bytes;
	std::vector<std::string> ve = split(cha, " ");

	this->addAllowedMethods(ve[0]);
	this->_protocols = ve[2];
	str.find("")
}