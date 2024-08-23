#ifndef ERROR_PAGE
# define ERROR_PAGE

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>


#include "Utils.hpp"

class ErrorPage
{
private:

public:
	static std::string getPage(int statusCode, std::map<int, std::string> errorPagesCustom);
	static std::string getErrorPagesCustom(int statusCode, std::map<int, std::string> errorPagesCustom);
};




#endif // ERROR_PAGE
