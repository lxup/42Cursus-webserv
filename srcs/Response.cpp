#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response() : _request(NULL), _blocServer(NULL), _blocLocation(NULL), _state(INIT)
{
}

Response::Response(Request *request, BlocServer *blocServer) : _request(request), _blocServer(blocServer), _state(INIT)
{
	initBlocLocation();
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


// UTIL RESPONSE ==============================


/**
 * @brief assigne le bon bloc location qui match avec la requete
 */
void Response::initBlocLocation(){
	const std::vector<BlocLocation>& locations = _blocServer->getLocations();
	std::string uri = _request->getUri();
	for (size_t i = 0; i < locations.size(); i++)
	{
		std::string pathLocation = locations[i].getPath();
		if (uri.find(pathLocation) == 0){
			Logger::log(Logger::DEBUG, "[findGoodLocation] Location found %s", pathLocation.c_str());
			_blocLocation = &locations[i];
			return ;
		}
	}
	Logger::log(Logger::DEBUG, "[findGoodLocation] PROUT NO Location not found");
	_blocLocation = NULL;
}


/**
 * @brief Fonction pour obtenir le type MIME basé sur l'extension de fichier
 */
std::string getMimeType(const std::string &path)
{
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
	if (idx != std::string::npos)
	{
		std::string ext = path.substr(idx);
		if (mimeTypes.find(ext) != mimeTypes.end())
		{
			return mimeTypes[ext];
		}
	}
	return "application/octet-stream"; // Type par défaut si l'extension est inconnue
}

/**
 * @brief check if the file exist
 */
bool Response::isRedirect()
{
	std::string uri = _request->getUri();
	std::string root;
	if (_blocLocation != NULL && _blocLocation->getRoot() != "")
		root = _blocLocation->getRoot();
	else
		root = _blocServer->getRoot();

	Logger::log(Logger::DEBUG, "roooot: %s", root);
	if (uri[uri.size() - 1] == '/' || uri == "/"){
		return false;
	}

	if (directoryExist((root + uri).c_str()))
	{
		_response = "HTTP/1.1 301 Moved Permanently\r\n";
		_response += "Location: http://" + _request->getHost() + uri + "/\r\n";
		_response += "Content-Length: 0\r\n";
		_response += "\r\n";

		Logger::log(Logger::DEBUG, "REDIRECT");
		return true;
	}
	Logger::log(Logger::DEBUG, "NO REDIRECT");


	return false;
}





// PATHS MANAGER =================


/**
 * @brief function qui renvoit tous les paths possible 
 * pour le bloc location qui correstpond a la requete
 * 
 */
std::vector<std::string> Response::getAllPathsLocation(){
	
	std::vector<std::string> allPathsLocations;
	std::string uri = _request->getUri();
	std::string root = _blocLocation->getRoot();
	std::vector<std::string> indexes = _blocLocation->getIndexes();

	if (_blocLocation == NULL)
		return std::vector<std::string>();

	if (root.empty())
		root = _blocServer->getRoot();

	// cas ou la requete demande un fichier direct
	if (uri[uri.size() - 1] != '/')
		allPathsLocations.push_back(root + uri);

	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string index = indexes[i];
		std::string path;
		if (uri == "/"){
			path = root + "/" + index;
		}
		else{
			path = root + uri + index;
		}
		allPathsLocations.push_back(path);
	}
	
	return allPathsLocations;
}









/**
 * @brief get the path of the ressource ask by the request
 * @example if the request is /index.html and the root is /var/www/html,
 * the path of the ressource is /var/www/html/index.html

 * TODO: gerer de tester tout les indexes
 */

std::vector<std::string> Response::getAllPathsServer(void)
{
	std::string uri = _request->getUri();												
	std::string root = _blocServer->getRoot();									
	std::vector<std::string> indexes = _blocServer->getIndexes();
	std::vector<std::string> allPaths;

	// cas ou la requete demande un fichier direct
	if (uri[uri.size() - 1] != '/')
		allPaths.push_back(root + uri);

	// cas ou la requete demande un dossier
	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string index = indexes[i];
		std::string path;
		if (uri == "/"){
			path = root + "/" + index;
		}
		else{
			path = root + uri + index;
		}
		allPaths.push_back(path);
	}


	return allPaths;
}







// GET METHOD ==============================


/**
 * @brief on check si la requete correspond a un bloc location
 * et on la gere
 */
void Response::manageLocation(){
	std::string page;

	if (_blocLocation == NULL)
		return ;
	
	// Bloc location
	std::vector<std::string> allPathsLocation = getAllPathsLocation();
	std::string path;


	// trouver le premier fichier qui existe dans allPathsLocation
	for (size_t i = 0; i < allPathsLocation.size(); i++)
	{
		Logger::log(Logger::DEBUG, "Trying to open file %s", allPathsLocation[i].c_str());
		path = allPathsLocation[i];
		if (fileExist(path))
			break;
	}

	Logger::log(Logger::DEBUG, "AHAH Opening file %s", path.c_str());
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::stringstream content;
		content << file.rdbuf();
		std::string body = content.str();

		page = "HTTP/1.1 200 OK\r\n";
		page += "Content-Type: " + getMimeType(path) + "\r\n";
		page += "Content-Length: " + intToString(body.size()) + "\r\n";
		page += "\r\n";
		page += body;

		file.close();
	}else if (directoryExist((_blocServer->getRoot() + _request->getUri()).c_str())){
		page = ErrorPage::getPage(403);
	}
	else{
		page = ErrorPage::getPage(404);
	}

	_response = page;
	setState(Response::FINISH);
}


/**
 * @brief on a checker la requete avec le bloc location et ca ne 
 * correspond pas, on va gerer la requete avec le bloc server
 */
void Response::manageServer(){
	std::string page;

	std::vector<std::string> allPaths = getAllPathsServer();
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
		std::string body = content.str();

		page = "HTTP/1.1 200 OK\r\n";
		page += "Content-Type: " + getMimeType(path) + "\r\n";
		page += "Content-Length: " + intToString(body.size()) + "\r\n";
		page += "\r\n";
		page += body;

		file.close();
	}else if (directoryExist((_blocServer->getRoot() + _request->getUri()).c_str())){
		page = ErrorPage::getPage(403);
	}
	else{
		page = ErrorPage::getPage(404);
	}

	_response = page;

}


/**
 * @brief handle the GET request
 */
void Response::handleGetRequest(void)
{

	manageLocation();
	if (_state != Response::FINISH)
		manageServer();
}













// MAIN RESPONSE ==============================
/**
 * @brief main function to return the response of the request _request
 */
std::string Response::getRawResponse(void)
{
	setState(Response::PROCESS);

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


void Response::setState(e_response_state state){
	if (this->_state == Response::FINISH)
		return (Logger::log(Logger::DEBUG, "[setState (response)] Response already finished"));
	if (this->_state == state)
		return (Logger::log(Logger::DEBUG, "[setState (response)] Response already in this state"));

	this->_state = state;

	if (this->_state == Response::INIT)
		Logger::log(Logger::DEBUG, "[setState (response)] Response INIT");
	else if (this->_state == Response::PROCESS)
		Logger::log(Logger::DEBUG, "[setState (response)] Response PROCESS");
	else if (this->_state == Response::FINISH)
		Logger::log(Logger::DEBUG, "[setState (response)] Response FINISH");
}

/* ************************************************************************** */