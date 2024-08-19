#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>



int main()
{
	std::vector<int> v; 
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	std::find(v.begin(), v.end(), 2) != v.end() ? std::cout << "Found" << std::endl : std::cout << "Not found" << std::endl;
}