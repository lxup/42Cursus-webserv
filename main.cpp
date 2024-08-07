#include <iostream>
#include <fstream>

int main()
{
	std::ifstream file(".gitignore");

	std::cout << file.good();
}