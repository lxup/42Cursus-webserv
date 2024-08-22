#include "Utils.hpp"

/* UTILS */

void printMsg(std::ostream &os, const char *msg, ...)
{
	const int initialBufferSize = 1024;
	std::vector<char> buffer(initialBufferSize);

	va_list args;
	va_start(args, msg);
	int size = vsnprintf(buffer.data(), initialBufferSize, msg, args);
	va_end(args);

	// Handling error
	if (size < 0)
	{
		// TODO: Handle error
		return;
	}

	// Resize the buffer if needed
	while (size >= static_cast<int>(buffer.size()))
	{
		buffer.resize(buffer.size() * 2);
		va_start(args, msg);
		size = vsnprintf(buffer.data(), buffer.size(), msg, args);
		va_end(args);
	}

	os << buffer.data() << std::endl;
}

/**
 * @brief renvoie true si le dossier existe
 */
bool directoryExist(const char *path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

/**
 * @brief renvoie true si le fichier existe
 */
bool fileExist(const std::string &name)
{
	std::ifstream file(name.c_str());
	return (file.good());
}

/**
 * @brief take string and return trimed line
 * 	example:
 * 		 string => "   bonjour   "
 * 		return : "bonjour"
 * @param line
 * @return std::string
 */
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

/**
 * @brief Splits a string into multiple substrings based on a delimiter.
 *
 * @example
 * std::string input = "Hello,World,!";
 * std::string delimiter = ",";
 * // result will contain {"Hello", "World", "!"}
 */
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

std::string unsignedIntToString(unsigned int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

/**
 * @brief Protected call to a function
 * @param ret return code of function to test
 * @param msg message in case of error
 * @param isFatal if true, the error is fatal (optional, default is true)
 * @return the return code of the function if >= 0
 */
int protectedCall(int ret, std::string msg, bool isFatal)
{
	if (ret < 0)
	{
		if (isFatal)
			Logger::log(Logger::FATAL, msg.c_str());
		else
			Logger::log(Logger::ERROR, msg.c_str());
	}
	return ret;
}


std::string		extractIp(std::string ipPort)
{
	return(ipPort.substr(0, ipPort.find(":")));
}

unsigned int	extractPort(std::string ipPort)
{
	return(std::atoi(ipPort.substr(ipPort.find(":") + 1).c_str()));
}
	
/**
 * @brief renvoie true si le fichier est vide
 */
bool isEmptyFile(){
	return (ConfigParser::countLineFile == 0);
}



// _____________________________ MODIFY EPOLL _____________________________
/**
 * WARNING: this function do not update flags for a socket, it only add socket to 
 * epoll with flags
 */
void addSocketEpoll(int epollFD, int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function");
}

/**
 * @brief
 * WARNING: this function over written previous flags
 * 
 */
void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_MOD, sockFD, &ev), "Error with epoll_ctl function");
}

void deleteSocketEpoll(int epollFD, int sockFD)
{
	epoll_event ev;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_DEL, sockFD, &ev), "Error with epoll_ctl function");
}

std::string getRedirectionMessage(int code)
{
	switch (code)
	{
	case 301:
		return "Moved Permanently";
	case 302:
		return "Found";
	case 303:
		return "See Other";
	case 307:
		return "Temporary Redirect";
	case 308:
		return "Permanent Redirect";
	default:
		return "Unknown";
	}
}