#include <iostream>
#include <string>
#include <regex>

bool checkIp(std::string str)
{
	int	i = 0;
	int	j = 0;

	for (int p = 0; p < 3; p++)
	{
		while (isdigit(str[i + j]))
		i++;
		if (i == 0 || i > 3 || str[j + i++] != '.')
			return (false);
		j += i;
		i = 0;
	}
	while (isdigit(str[i + j]))
		i++;
	if (i == 0 || i > 3 || str[j + i++] != ':')
		return (false);
	j += i;
	i = 0;
	while (isdigit(str[i + j]))
		i++;
	if (i == 0 || i > 5)
		return (false);
	return (true);
}

int main()
{
	if (checkIp("127.747.0.1:8080"))
		std::cout << "caca";
}