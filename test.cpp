
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cstdarg>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>
#include <sys/stat.h>



bool directoryExist(const char *path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

int main(int argc, char const *argv[])
{
	std::string path = "./test.cpp";

	if (directoryExist(path.c_str()))
		std::cout << "Directory exist" << std::endl;
	else
		std::cout << "Directory does not exist" << std::endl;
	return 0;
}
