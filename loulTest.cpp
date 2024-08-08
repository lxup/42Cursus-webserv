#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <string>

bool fileExist(const std::string &name)
{
	std::ifstream file(name.c_str());
	return (file.good());
}

bool fileExistVector(std::vector<std::string> _files)
{
	std::vector<std::string>::const_iterator it;

	for (it = _files.begin(); it != _files.end(); it++)
		if (!fileExist(*it))
			return (false);
	return (true);
}

int main()
{
	std::vector<std::string> _files;

	_files.push_back("./Makefile");
	_files.push_back("./a.ou");
	_files.push_back("./tmp/www/pouet.html");
	_files.push_back("./tmp/www/webserv.html");

	std::cout << fileExistVector(_files);

}