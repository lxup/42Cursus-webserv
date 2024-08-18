
#include "Listen.hpp"

// --------------------- GENERAL ---------------------

Listen::Listen() {}

Listen::Listen(std::string token) {
	
	std::vector<std::string> ip = split(token, ":");

	if (ip.size() == 1){
		if (ip[0].find(".") != std::string::npos){
			_ip = ip[0];
			_port = 80;
		}else{
			_ip = "0.0.0.0";
			_port = std::atoi(ip[0].c_str());
		}
	}else if (ip.size() == 2){
		_ip = ip[0];
		_port = std::atoi(ip[1].c_str());
	}else{
		Logger::log(Logger::FATAL, "Invalid listen value: %s", token.c_str());
	}
	_ipPortJoin = _ip + ":" + unsignedIntToString(_port);
	if (!checkIpPort())
		Logger::log(Logger::FATAL, "Invalid Ip/Port value: %s", token.c_str());
}

Listen::~Listen() {}

// --------------------- CHECKER ---------------------

bool Listen::checkIpPort()
{
	int	i = 0;
	int	j = 0;
	std::string str = _ipPortJoin;

	for (int p = 0; p < 3; p++)
	{
		while (isdigit(str[i + j]))
		i++;
		if (i == 0 || i > 3 || str[j + i++] != '.')
			return (false);
		j += i;
		i = 0;
	}
	while (isdigit(str[i + j]))
		i++;
	if (i == 0 || i > 3 || str[j + i++] != ':')
		return (false);
	if (_port > 65535)
		return (false);
	return (true);
}

