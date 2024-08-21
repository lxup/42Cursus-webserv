#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response( void ){
}

Response::Response(Request* request, BlocServer blocServer) : _request(request), _blocServer(blocServer)
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
 * @brief handle the POST request
 */
// bool Response::isRedirect(){
//     std::string path = _request.getUri();
//     std::string root = _blocServer.getRoot();
// }

/**
 * @brief get the path of the ressource ask by the request
 * @example if the request is /index.html and the root is /var/www/html, 
 * the path of the ressource is /var/www/html/index.html

 * TODO: gerer de tester tout les indexes
 */

// std::vector<std::string> Response::getPathRessource(void) {
//     std::string path = _request.getUri();
//     std::string root = _blocServer.getRoot();
//     std::vector<std::string> indexes = _blocServer.getIndexes();
//     std::vector<std::string> pathRessources;

//     for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++) {
//         std::string index = *it;
//         std::string pathRessource;
//         if (path == "/") {
//             pathRessource = root + index;
//         } else {
//             pathRessource = root + path;
//         }
//         pathRessources.push_back(pathRessource);
//     }

//     return pathRessources;
// }
/**
 * @brief handle the GET request
 */
void Response::handleGetRequest(void) {
        std::string content;

        // std::vector<std::string> pathRessources = getPathRessource();
        // std::string path;
        // std::ifstream file(path.c_str());

        // if (file.is_open()) {
        //     content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        //     file.close();

        //     page = "HTTP/1.1 200 OK\r\n";
        //     page += "Content-Type: text/html\r\n";
        //     page += "Content-Length: " + intToString(content.size()) + "\r\n";
        //     page += "\r\n"; // Terminer les en-têtes
        //     page += content;
        // } else {
        //     content = "<html><body><h1>404 Not Found</h1></body></html>";
        //     page = "HTTP/1.1 404 Not Found\r\n";
        //     page += "Content-Type: text/html\r\n";
        //     page += "Content-Length: " + intToString(content.size()) + "\r\n";
        //     page += "\r\n"; // Terminer les en-têtes
        //     page += content;
        // }

    // Logger::log(Logger::DEBUG, "RESPONSE : %s", page.c_str());
}
/**
 * @brief main function to return the response of the request _request
 */
std::string Response::getRawResponse(void) {

    // print some information about the request
    // Logger::log(Logger::DEBUG, "request method : %s", _request.getEr);

    // Logger::log(Logger::DEBUG, "request URI : %s", _request.getUri().c_str());
    // if (isRedirect())
    //     ;
    // if (_request.getMethod == "GET")
    //     handleGetRequest();
    // else if (_request.getMethod == "POST")
    //     handlePostRequest();
    // else if (_request.getMethod == "DELETE")
    //     handleDeleteRequest();
    // else if (_request.getMethod == "PUT")
    //     handlePutRequest();
    // else
    //     handleOtherRequest();

    return _response;


}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */