#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename)
	: _filename(filename)
{
	_parse();
}

ConfigParser::~ConfigParser(void) {}

e_boolMod strToBool(std::string &str)
{
	if (str == "on")
		return (TRUE);
	else if (str == "off")
		return (FALSE);
	return (BAD);
}

std::string trimLine(std::string &line)
{
	const std::string white_chars = " \t\n\r\f\v";
	std::string result;

	int start = 0;
	while (white_chars.find(line[start]) != std::string::npos)
		start++;
	int end = line.size() - 1;
	while (end >= 0 && white_chars.find(line[end]) != std::string::npos)
		end--;

	for (int i = start; i <= end; i++)
		result.push_back(line[i]);

	return (result);
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> result;
	s = trimLine(s);
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
		s = trimLine(s);
	}
	result.push_back(s);
	return (result);
}

Location ConfigParser::getLocationConfig(std::ifstream &configFile, std::string &path)
{
	Location location;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseLocation = false;

	location.setLocation(path);
	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key == "}"){
			isCloseLocation = true;
			break;
		}
		else if (key == "root" && !tokens[1].empty())
			location.setRoot(tokens[1]);
		else if (key == "autoindex" && !tokens[1].empty())

			location.setAutoIndex(strToBool(tokens[1]));
		else if (key == "rewrite" && !tokens[1].empty())
			location.setRewrite(tokens[1]);
		else if (key == "alias" && !tokens[1].empty())
			location.setAlias(tokens[1]);
		else if (key == "allowedMethods" && !tokens[1].empty())
		{
			location.incrementCounter("allowedMethods");
			for (size_t i = 1; i < tokens.size(); i++)
				location.addAllowedMethods(tokens[i]);
		}
		else if (key == "index" && !key.empty())
		{
			location.incrementCounter("files");
			for (size_t i = 1; i < tokens.size(); i++)
				location.addFile(tokens[i]);
		}else
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
	}
	if (!isCloseLocation)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename.c_str());

	return (location);
}


Server ConfigParser::getServerConfig(std::ifstream &configFile)
{
	Server server;
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseServer = false;

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		key = tokens[0];
		if (key == "}"){
			isCloseServer = true;
			break;
		}
		if (key.empty())
			continue;
		if (tokens.size() == 3 && key == "location" && tokens[2] == "{"){
			Location location = getLocationConfig(configFile, tokens[1]);
			location.checkDoubleLine();
			server.addLocation(location);
		}
		else if (key == "listen")
			server.setPort(std::atoi(tokens[1].c_str()));
		else if (key == "server_name")
			server.setServerName(tokens[1]);
		else if (key == "root")
			server.setRoot(tokens[1]);
		else if (key == "client_max_body_size")
			server.setClientMaxBodySize(std::atoi(tokens[1].c_str()));
		else if (key == "error_page"){
			server.incrementCounter(tokens[1]);
			server.addErrorPages(std::atoi(tokens[1].c_str()), tokens[2]);
		}
		else
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
	}
	if (isCloseServer == false)
		throw WebservException(Logger::FATAL, "Missing } in %s", _filename.c_str());

	return (server);
}

void ConfigParser::_parse(void)
{
	Logger::log(Logger::DEBUG, "Parsing config file: %s", _filename.c_str());
	std::ifstream configFile(_filename.c_str());
	std::vector<std::string> tokens;
	std::string line;
	std::stack<e_context> context;

	if (!configFile.is_open())
	{
		Logger::log(Logger::ERROR, "The config file can't be open");
		exit(EXIT_FAILURE); // A MODIFIER
	}

	while (std::getline(configFile, line))
	{
		line = trimLine(line);
		if (line.empty() || line[0] == '#')
			continue;
		tokens = split(line, " ");
		if (tokens[0] == "server" && tokens[1] == "{")
		{
			context.push(SERVER);
			Server server = getServerConfig(configFile);
			server.checkDoubleLine();
			_servers.push_back(server);
		}else{
			throw WebservException(Logger::FATAL, "Invalid line in %s file: %s", _filename.c_str(), line.c_str());
		}
	}
	checkAttribut();
	configFile.close();

	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "============ SERVER " << i << " ===========\n"
				  << std::endl;
		_servers[i].printServer();
	}
}

void ConfigParser::checkAttribut()
{
	std::vector<Server>::iterator it;

	for (it = _servers.begin(); it != _servers.end(); it++)
		(*it).checkAttribut();
}
