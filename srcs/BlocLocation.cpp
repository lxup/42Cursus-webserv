#include "BlocLocation.hpp"

BlocLocation::BlocLocation() : _location(""), _root(""), _rewrite(""), _alias(""), _autoindex(FALSE)
{
	_allowedMethods.push_back(GET);
	_counter["location"] = 0;
	_counter["root"] = 0;
	_counter["rewrite"] = 0;
	_counter["alias"] = 0;
	_counter["files"] = 0;
	_counter["allowedMethods"] = 0;
	_counter["autoindex"] = 0;
}

BlocLocation::~BlocLocation() {}

void BlocLocation::addAllowedMethods(std::string &token)
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
	if (std::find(_allowedMethods.begin(), _allowedMethods.end(), met) != _allowedMethods.end())
		return ;
	_allowedMethods.push_back(met);
}

bool BlocLocation::fileExistVector()
{
	std::vector<std::string>::const_iterator it;

	for (it = _files.begin(); it != _files.end(); ++it)
		if (!fileExist(*it))
			return (false);
	return (true);
}

bool BlocLocation::methodsExist()
{
	std::vector<e_Methods>::const_iterator it = _allowedMethods.begin();

	if (!(*it))
		return (true);
	for (; it != _allowedMethods.end(); ++it)
		if (*it == BAD_MET)
			return (false);
	return (true);
}

void BlocLocation::checkValue()
{
	if (!methodsExist())
		throw WebservException(Logger::FATAL, "Invalid methods value");
}

void BlocLocation::printLocation(void) const
{
	std::cout << "Path: " << _location << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Rewrite: " << _rewrite << std::endl;
	std::cout << "Alias: " << _alias << std::endl;
	std::cout << "Files: " << std::endl;
	for (std::vector<std::string>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		std::cout << "	- " << *it << std::endl;
	std::cout << "Allowed methods: " << std::endl;
	for (std::vector<e_Methods>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
	{
		std::cout << "	- ";
		if (*it == GET)
			std::cout << "GET" << std::endl;
		else if (*it == POST)
			std::cout << "POST" << std::endl;
		else if (*it == DELETE)
			std::cout << "DELETE" << std::endl;
		// std::cout << *it << std::endl;
	}

	if (_autoindex == TRUE)
		std::cout << "Autoindex: TRUE" << std::endl;
	else
		std::cout << "Autoindex: FALSE" << std::endl;
}

void BlocLocation::checkDoubleLine()
{

	std::map<std::string, int>::iterator it;

	for (it = _counter.begin(); it != _counter.end(); ++it)
	{
		if (it->second > 1)
		{
			throw WebservException(Logger::FATAL, "Dupplicate line in location context: %s", it->first.c_str());
		}
	}
}
