#include <iostream>
#include <string>
#include <regex>

int main()
{
	std::string oui = "void addAllowedMethods(std::string &token);";

	oui[oui.find("string")] = (char)"|";
}