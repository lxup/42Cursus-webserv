#include "Utils.hpp"
#include <stdarg.h>

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
