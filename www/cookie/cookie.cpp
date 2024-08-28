
#include <string>
#include <iostream>


//std::string getCookieLine(){
//	std::string cookieLine;
//	std::string cookieHeader = _request->getHeaders()["Cookie"];

//	if (_request->getHeaders().find("Cookie") != _request->getHeaders().end() && (cookieHeader.empty() || cookieHeader.find("id=") == std::string::npos)){
//		// selectionner seulement le id
//		int id = cookie.substr(cookie.find("id=") + 3);
//		int pos = cookie.find(";");
//		if (pos != std::string::npos)
//			id = cookie.substr(0, pos);
		
//		//std::cout << "Cookie already there: " << cookie << std::endl;
//		//cookieLine = cookie;
//	}
//	else{
//		int newId = generateId();

//		cookieLine = "Set-Cookie: id=" + generateId() + "; Expires=" + generateExpirationDate() + "; Path=/\r\n";
//		std::cout << "Cookie created: " << cookieLine << std::endl;
//	}
//	return cookieLine;
//}


// generate html page in cpp 

std::string page = "<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>Document</title>\n"
"</head>\n"
"<body>\n"
"    <h1>My first HTML page</h1>\n"
"    <p>My first paragraph.</p>\n"
"</body>\n"
"</html>\n";

std::string header = "Content-Type: text/html\r\n\r\n";

int main() {
	std::cout << header << page;
	return 0;
}
