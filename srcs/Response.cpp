#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(void)
{
}

Response::Response(Request *request, BlocServer *blocServer) : _request(request), _blocServer(blocServer)
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
 * @brief Fonction pour obtenir le type MIME basé sur l'extension de fichier
 */
std::string getMimeType(const std::string& path) {
    std::map<std::string, std::string> mimeTypes;

    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";
		// todo ajouter d'autre extension

    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos) {
        std::string ext = path.substr(idx);
        if (mimeTypes.find(ext) != mimeTypes.end()) {
            return mimeTypes[ext];
        }
    }
    return "application/octet-stream"; // Type par défaut si l'extension est inconnue
}

/**
 * @brief handle the POST request
 */
bool Response::isRedirect()
{
	std::string uri = _request->getUri(); // /
	std::string root = _blocServer->getRoot(); // ./www/main

	if (uri[uri.size() - 1] != '/' || uri == "/")
		return false;

	if (directoryExist((root + uri).c_str()))
	{
		_response = "HTTP/1.1 301 Moved Permanently\r\n";
		_response += "Location: " + uri + "/\r\n";
		_response += "\r\n";
		return true;
	}

	return false;
}

/**
 * @brief get the path of the ressource ask by the request
 * @example if the request is /index.html and the root is /var/www/html,
 * the path of the ressource is /var/www/html/index.html

 * TODO: gerer de tester tout les indexes
 */

std::vector<std::string> Response::getAllPaths(void)
{
	std::string uri = _request->getUri(); // /
	std::string root = _blocServer->getRoot(); // ./www/main
	std::vector<std::string> indexes = _blocServer->getIndexes(); // index.html
	std::vector<std::string> allPaths;

	// cas ou la requete demande un fichier
	if (uri[uri.size() - 1] != '/'){
		allPaths.push_back(root + uri);
		return allPaths;
	}

	// cas ou la requete demande un dossier
	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
	{
		std::string index = *it;
		std::string pathRessource;
		if (uri == "/"){
			pathRessource = root + "/" + index;
		}
		else{
			pathRessource = root + uri + index;
		}
		allPaths.push_back(pathRessource);
	}

	return allPaths;
}
/**
 * @brief handle the GET request
 */
void Response::handleGetRequest(void)
{
	std::string page;

	std::vector<std::string> allPaths = getAllPaths();
	std::string path;

	// trouver le premier fichier qui existe dans allPaths
	for (size_t i = 0; i < allPaths.size(); i++)
	{
		Logger::log(Logger::DEBUG, "Trying to open file %s", allPaths[i].c_str());
		path = allPaths[i];
		if (fileExist(path))
			break;
	}

	Logger::log(Logger::DEBUG, "AHAH Opening file %s", path.c_str());
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::stringstream content;
		content << file.rdbuf();
		std::string buff = content.str();

		page = "HTTP/1.1 200 OK\r\n";
		page += "Content-Type: " + getMimeType(path) + "\r\n";
		page += "Content-Length: " + intToString(buff.size()) + "\r\n";
		page += "\r\n"; // Terminer les en-têtes
		page += buff;

		file.close();
	}
	else
	{
		// error 404 not found
		page = ErrorPage::getPage(404);
	}

	_response = page;
}
/**
 * @brief main function to return the response of the request _request
 */
std::string Response::getRawResponse(void)
{

	// print some information about the request

	if (isRedirect())
		return _response;
	if (_request->getMethod() == "GET")
		handleGetRequest();
	else
		_response = "HTTP/1.1 405 Method Not Allowed\r\n";
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