#include "BlocLocation.hpp"

// ------------------------------- GENERAL --------------------------------
BlocLocation::BlocLocation(std::string filename) : _location(""), _root(""), _rewrite(""), _alias(""), _autoindex(FALSE), _filename(filename)
{
	// todo changer ca parce que par defautl ya les trois
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

// ------------------------------- UTIL --------------------------------	
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

e_boolMod BlocLocation::strToBool(std::string &str)
{
	if (str == "on")
		return (TRUE);
	else if (str == "off")
		return (FALSE);
	return (BAD);
}

// ------------------------------- CHECKER --------------------------------

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

//void BlocLocation::checkValue()
//{
//	if (!methodsExist())
//		throw WebservException(Logger::FATAL, "Invalid methods value");
//}




// ------------------------------- PARSING --------------------------------
/**
 * @brief This function checks if a line in the configuration file is a good directive in BlocLocation bloc (autoindex par ex)
 * It updates the location object with the corresponding values if the line is valid.
 * 
 * @param location  The BlocLocation object to update.
 * @param tokens  A vector of strings containing the tokens of the line.
 * @param key The key of the line. (the first token)
 */
bool BlocLocation::isValidLineLocation(std::vector<std::string>& tokens, std::string& key)
{
	if (key == "root" && !tokens[1].empty())
		setRoot(tokens[1]);
	else if (key == "autoindex" && !tokens[1].empty())
		setAutoIndex(strToBool(tokens[1]));
	else if (key == "rewrite" && !tokens[1].empty())
		setRewrite(tokens[1]);
	else if (key == "alias" && !tokens[1].empty())
		setAlias(tokens[1]);
	else if (key == "allow_methods" && !tokens[1].empty())
	{
		incrementCounter("allowedMethods");
		for (size_t i = 1; i < tokens.size(); i++)
			addAllowedMethods(tokens[i]);
	}
	else if (key == "index" && !key.empty())
	{
		incrementCounter("files");
		for (size_t i = 1; i < tokens.size(); i++)
			addFile(tokens[i]);
	}
	else
		return false;
	return true;
}


/**
 * @brief parse a BlocLocation bloc
 * 
 */
BlocLocation BlocLocation::getLocationConfig(std::ifstream &configFile, std::string &path)
{
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseLocation = false;

	setLocation(path);
	while (std::getline(configFile, line)){
		ConfigParser::countLineFile++;
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key[0] == '}'){
			isCloseLocation = true;
			break;
		}
		if (isValidLineLocation(tokens, key))
			continue;
		else
			throw WebservException(Logger::FATAL, "Invalid line: \"%s\" in file: %s:%d", line.c_str(), _filename.c_str(), ConfigParser::countLineFile);
	}
	if (!isCloseLocation)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename.c_str());
	
	return (*this);
}






// ------------------------------- PRINT --------------------------------
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
