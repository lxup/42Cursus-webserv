#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define BUFFER_SIZE 4096

int server()
{

	int fdSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(fdSocket, (const sockaddr *)(&address), sizeof(address));

	listen(fdSocket, 10);

	bool active = true;
	int connection;
	while (active)
	{
		unsigned long resultLen = sizeof(sockaddr);
		std::cout << "Listening on Port: " << PORT << std::endl;
		connection = accept(fdSocket, (struct sockaddr *)(&address), (socklen_t *)&resultLen);

		char buffer[BUFFER_SIZE];
		ssize_t bytesRead = read(connection, buffer, BUFFER_SIZE);
		std::cout << "Le message fait: " << bytesRead << " characteres" << std::endl;
		std::cout << buffer << std::endl;

		std::string content = "<h1>Bonjour, je suis un serveur HTTP tout simple!</h1>";
		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(content.length()) + "\n\n" + content;
		send(connection, response.c_str(), response.size(), 0);
		close(connection);
	}

	close(fdSocket);

	return (EXIT_SUCCESS);
}

int main()
{
	server();
	return 0;
}
