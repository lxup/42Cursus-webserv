#ifndef ERROR_PAGE
# define ERROR_PAGE

#include <iostream>
#include <map>
#include <string>


#include "Utils.hpp"


class ErrorPage
{
private:

public:
	static std::string getMessage(int statusCode);
	static std::string getPage(int statusCode);
};




#endif // ERROR_PAGE
