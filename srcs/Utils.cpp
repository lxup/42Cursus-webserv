#include "Utils.hpp"

/* WEBSERV EXCEPTION */
WebservException::WebservException(int errnoNum, const char *msg, ...)
{
	int bufferSize = 1024;
	va_list args;
	va_start(args, msg);
	char buff[bufferSize];
	int size = vsnprintf(buff, bufferSize, msg, args);
	va_end(args);

	if (size < 0)
	{
		// TODO: throw exception
	}

	while (size >= bufferSize)
	{
		bufferSize *= 2;
		char newBuff[bufferSize];
		va_start(args, msg);
		size = vsnprintf(newBuff, bufferSize, msg, args);
		va_end(args);
		if (size < 0)
		{
			// TODO: throw exception
		}
	}

	_errno = errnoNum;
	_msg = buff;
	if (_errno)
		_msg += " (" + std::string(strerror(_errno)) + ")";
}

WebservException::WebservException(Logger::LogLevel logLevel, const char *msg, ...) : _logLevel(logLevel)
{
	int bufferSize = 1024;
	va_list args;
	va_start(args, msg);
	char buff[bufferSize];
	int size = vsnprintf(buff, bufferSize, msg, args);
	va_end(args);

	if (size < 0)
	{
		// TODO: throw exception
	}

	while (size >= bufferSize)
	{
		bufferSize *= 2;
		char newBuff[bufferSize];
		va_start(args, msg);
		size = vsnprintf(newBuff, bufferSize, msg, args);
		va_end(args);
		if (size < 0)
		{
			// TODO: throw exception
		}
	}
	_msg = buff;
}

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

bool directoryExist(const char *path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

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
 * @brief affiche l'event qui a ete detecte
 */
void printEvent(int fd, uint32_t event){
	if (event & EPOLLIN)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLIN | FD: %d", fd);
	if (event & EPOLLOUT)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLOUT | FD: %d", fd);
	if (event & EPOLLRDHUP)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLRDHUP | FD: %d", fd);
	if (event & EPOLLERR)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLERR | FD: %d", fd);
	if (event & EPOLLHUP)
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: EPOLLHUP | FD: %d", fd);
	else
		Logger::log(Logger::DEBUG, "NOUVEL EVENT: UNKNOWN | FD: %d", fd);
}