#include <iostream>
#include <string>
#include <regex>

int main()
{
	if (std::regex_match("127.0.0.1:8080", std::regex("([0-9]{1,3}\\.){3}[0-9]{1,3}:[0-9]{1,5}")))
		std::cout << "caca";
}