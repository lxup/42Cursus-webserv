#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(Request request, BlocServer blocServer) : _request(request), _blocServer(blocServer)
{
}



/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response()
{
}



/*
** --------------------------------- METHODS ----------------------------------
*/

/**
 * @brief main function to return the response of the request _request
 */
std::string Response::getResponse(void) {
    std::string page;
    std::string content;
    Logger::log(Logger::DEBUG, "request URI : %s", _request.getUri().c_str());

    if (_request.getMethod() == "GET" && _request.getUri().find("favicon") == std::string::npos) {

        std::string path;
        if (_request.getUri() == "/")
            path = _blocServer.getRoot() + '/';
        else
            path = _blocServer.getRoot() + _request.getUri();
        path += _blocServer.getIndexes()[0];
				Logger::log(Logger::DEBUG, "index: %s", _blocServer.getIndexes()[0].c_str());
        Logger::log(Logger::DEBUG, "PATH: %s", path.c_str());
        std::ifstream file(path.c_str());

        if (file.is_open()) {
            content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            page = "HTTP/1.1 200 OK\r\n";
            page += "Content-Type: text/html\r\n";
            page += "Content-Length: " + intToString(content.size()) + "\r\n";
            page += "\r\n"; // Terminer les en-têtes
            page += content;
        } else {
            content = "<html><body><h1>404 Not Found</h1></body></html>";
            page = "HTTP/1.1 404 Not Found\r\n";
            page += "Content-Type: text/html\r\n";
            page += "Content-Length: " + intToString(content.size()) + "\r\n";
            page += "\r\n"; // Terminer les en-têtes
            page += content;
        }

    } else {
        content = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        page = "HTTP/1.1 405 Method Not Allowed\r\n";
        page += "Content-Type: text/html\r\n";
        page += "Content-Length: " + intToString(content.size()) + "\r\n";
        page += "\r\n"; // Terminer les en-têtes
        page += content;
    }

    Logger::log(Logger::DEBUG, "RESPONSE : %s", page.c_str());
    return page;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */