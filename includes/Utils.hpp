
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cstdarg>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <filesystem>



#include "Logger.hpp"
#include "ConfigParser.hpp"
#include "ErrorPage.hpp"




/* UTILS */
void printMsg(std::ostream &os, const char *msg, ...);
bool directoryExist(const char *path);
bool fileExist(const std::string &name);
std::string trimLine(std::string &line);
std::vector<std::string> split(std::string s, std::string delimiter);
std::string unsignedIntToString(unsigned int value);
std::string intToString(int value);
std::string	getExtension(const std::string &path, bool includeDot = true);

int protectedCall(int ret, std::string msg, bool isFatal = true);

std::string extractIp(std::string ipPort);
unsigned int extractPort(std::string ipPort);
bool isEmptyFile();
std::string getErrorMessage(int code);
std::string intToHexa(ssize_t num);
std::string getMimeType(const std::string &path);

// epoll utils
void addSocketEpoll(int epollFD, int sockFD, uint32_t flags);
void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags);
void deleteSocketEpoll(int epollFD, int sockFD);

// list directory
std::string buildPage(std::vector<std::string> files, std::string path);
void cleanPath(std::string& path);
bool is_path_within_root(const std::string& root, std::string& path) ;
std::string listDirectory(std::string path, std::string root);

class IntException : public std::exception {
private:
    int _code;
public:
   IntException(int code) : _code(code) {}
   virtual ~IntException() throw() {}

    // virtual const char* what() const throw() {
    //     return getErrorMessage(_code).c_str();
    // }

    int code() const {
		return _code;
	}
};


#endif // UTILS_HPP