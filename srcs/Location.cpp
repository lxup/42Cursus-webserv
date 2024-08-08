#include "Location.hpp"

Location::Location() : _location(""), _root(""), _rewrite(""), _alias(""), _allowedMethods(GET), _autoindex(FALSE)
{
	_counter["location"] = 0;
	_counter["root"] = 0;
	_counter["rewrite"] = 0;
	_counter["alias"] = 0;
	_counter["files"] = 0;
	_counter["allowedMethods"] = 0;
	_counter["autoindex"] = 0;
}

Location::~Location() {}

void Location::addAllowedMethods(std::string &token)
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
	_allowedMethods.push_back(met);
}

bool Location::fileExistVector()
{
	std::vector<std::string>::const_iterator it;

	for (it = _files.begin(); it != _files.end(); ++it)
		if (!fileExist(*it))
			return (false);
	return (true);
}

bool Location::methodsExist()
{
	std::vector<e_Methods>::const_iterator it = _allowedMethods.begin();

	if (!(*it))
		return (true);
	for (; it != _allowedMethods.end(); ++it)
		if (*it == BAD_MET)
			return (false);
	return (true);
}

void Location::checkValue()
{
	if (!directoryExist(_location.c_str()))
		throw WebservException(Logger::FATAL, "Invalid path value");
	if (!directoryExist(_root.c_str()) && !_root.empty())
		throw WebservException(Logger::FATAL, "Invalid root location value");
	if (!directoryExist(_alias.c_str()) && !_alias.empty())
		throw WebservException(Logger::FATAL, "Invalid alias value");
	if (!fileExistVector())
		throw WebservException(Logger::FATAL, "Invalid file value");
	if (!methodsExist())
		throw WebservException(Logger::FATAL, "Invalid methods value");
}

void Location::printLocation(void) const
{
	std::cout << "Location: " << _location << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Rewrite: " << _rewrite << std::endl;
	std::cout << "Alias: " << _alias << std::endl;
	std::cout << "Files: " << std::endl;
	for (std::vector<std::string>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		std::cout << *it << std::endl;
	std::cout << "Allowed methods: " << std::endl;
	for (std::vector<e_Methods>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
	{
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

void Location::checkDoubleLine()
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
