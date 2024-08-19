#include <iostream>
#include <string>
#include <regex>
#include <algorithm>

std::string trimLine(std::string &line)
{
	const std::string white_chars = " \t\n\r\f\v";
	std::string result;

	int start = 0;
	while (white_chars.find(line[start]) != std::string::npos)
		start++;
	int end = line.size() - 1;
	while (end >= 0 && white_chars.find(line[end]) != std::string::npos)
		end--;

	for (int i = start; i <= end; i++)
		result.push_back(line[i]);

	return (result);
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> result;
	s = trimLine(s);
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
		s = trimLine(s);
	}
	result.push_back(s);
	return (result);
}

int main()
{
	std::string oui = "GET / HTTP/1.1\nUser-Agent: PostmanRuntime/7.41.1\nAccept: */*\nPostman-Token: e4cb2627-d891-46b7-bec3-3383598233d1\nHost: localhost:1312\nAccept-Encoding: gzip, deflate, br\nConnection: keep-alive";
	std::vector<std::string> ve = split(oui, " ");
	auto it = std::find(ve.begin(), ve.end(), "salut");
	std::cout << *it << std::endl;
}