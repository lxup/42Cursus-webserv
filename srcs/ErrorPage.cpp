
#include "ErrorPage.hpp"

std::string ErrorPage::getMessage(int statusCode){
	std::string message;
	switch (statusCode)
	{
	case 400:
		message = "Bad Request";
		break;
	case 401:
		message = "Unauthorized";
		break;
	case 403:
		message = "Forbidden";
		break;
	case 404:
		message = "Not Found";
		break;
	case 405:
		message = "Method Not Allowed";
		break;
	case 406:
		message = "Not Acceptable";
		break;
	case 407:
		message = "Proxy Authentication Required";
		break;
	case 408:
		message = "Request Timeout";
		break;
	case 409:
		message = "Conflict";
		break;
	case 410:
		message = "Gone";
		break;
	case 411:
		message = "Length Required";
		break;
	case 412:
		message = "Precondition Failed";
		break;
	case 413:
		message = "Payload Too Large";
		break;
	case 414:
		message = "URI Too Long";
		break;
	case 415:
		message = "Unsupported Media Type";
		break;
	case 429:
		message = "Too Many Requests";
		break;
	case 500:
		message = "Internal Server Error";
		break;
	case 501:
		message = "Not Implemented";
		break;
	case 502:
		message = "Bad Gateway";
		break;
	case 503:
		message = "Service Unavailable";
		break;
	case 504:
		message = "Gateway Timeout";
		break;
	case 505:
		message = "HTTP Version Not Supported";
		break;
	default:
		message = "Unknown Error";
		break;
	}
	return message;
}


/**
 * @brief Get the html error page related to the status code
 * Replace TITLE with the error code
 * Replace ERROR by the status code
 * Replace MESSAGE by the message
 * 
 * @param statusCode 
 * @return std::string 
 */
std::string ErrorPage::getPage(int statusCode){
	std::string message = ErrorPage::getMessage(statusCode);
	std::string body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>TITLE</title><style>@import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');body{height: 100vh;margin: 0;padding: 0;display: flex;justify-content: center;align-items: center;background-color: #f1f1f1;font-family: \"Inter\", sans-serif;font-optical-sizing: auto;flex-direction: column;}.wrapper{position: relative;}.mainTitle{text-align: center;position: relative;font-size: 3rem;}.mainIcon{left: 50%;transform: translate(-50%, -100%);position: absolute;}.mainText{position: absolute;top: 50%;left: 50%;transform: translate(-50%, 120%);text-align: center;width: 100vw;}.subtitle{position: absolute;bottom: 0;}</style></head><body><div class=\"wrapper\"><lord-icon src=\"https://cdn.lordicon.com/usownftb.json\" trigger=\"loop\" delay=\"2000\" colors=\"primary:#000000,secondary:#000000\" style=\"width:150px;height:150px\" class=\"mainIcon\"></lord-icon><h1 class=\"mainTitle\">Error ERROR</h1><p class=\"mainText\">MESSAGE</p></div></body><script src=\"https://cdn.lordicon.com/lordicon.js\"></script></html>";

	body.replace(body.find("TITLE"), 5, intToString(statusCode));
	body.replace(body.find("ERROR"), 5, intToString(statusCode));
	body.replace(body.find("MESSAGE"), 7, message);

	std::string responseLine = "HTTP/1.1 " + intToString(statusCode) + " " + message + "\r\n";
	std::string headers = "Content-Type: text/html\r\nContent-Length: " + intToString(body.size()) + "\r\n\r\n";
	return responseLine + headers + body;
}