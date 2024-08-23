#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response() : _request(NULL), _blocServer(NULL), _blocLocation(NULL), _state(INIT), _fileFd(-1)
{
}

Response::Response(Request *request) : _request(request), _blocServer(request->getServer()), _blocLocation(request->getLocation()), _state(INIT), _fileFd(-1)
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

// UTIL RESPONSE ==============================

/**
 * @brief check if the folder exist and if he does not have a / at the end
 */
bool Response::isRedirect()
{
	std::string uri = _request->getUri();
	std::string root;
	bool isLoc;

	isLoc = _blocLocation == NULL ? false : true;

	if (isLoc && !_blocLocation->getRewrite().second.empty())
	{
		std::pair<int, std::string> rewrite = _blocLocation->getRewrite();
		_response = "HTTP/1.1 " + intToString(rewrite.first) + " " + getErrorMessage(rewrite.first) + "\r\n";
		_response += "Location:" + rewrite.second + "\r\n";
		_response += "Content-Length: 0\r\n";
		_response += "\r\n";
		return (true);
	}

	if (isLoc && !_blocLocation->getRoot().empty())
		root = _blocLocation->getRoot();
	else
		root = _blocServer->getRoot();

	// si l'uri fini par / ou est egal a / on ne redirige pas
	if (uri[uri.size() - 1] == '/' || uri == "/")
	{
		return false;
	}

	if (directoryExist((root + uri).c_str()) || (isLoc && directoryExist(_blocLocation->getAlias().c_str())))
	{
		std::string host = _request->getHeaders()["Host"];
		_response = "HTTP/1.1 301 Moved Permanently\r\n";
		_response += "Location: http://" + host + uri + "/\r\n";
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
std::vector<std::string> Response::getAllPathsLocation()
{

	std::vector<std::string> allPathsLocations;
	std::string uri = _request->getUri();
	std::string root = _blocLocation->getRoot();
	std::string alias = _blocLocation->getAlias();
	std::vector<std::string> indexes = _blocLocation->getIndexes();
	bool isAlias = false;

	if (_blocLocation == NULL)
		return std::vector<std::string>();

	if (!alias.empty())
	{
		root = alias;
		isAlias = true;
	}

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
		else if (isAlias){
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
		if (uri == "/")
		{
			path = root + "/" + index;
		}
		else
		{
			path = root + uri + index;
		}
		allPaths.push_back(path);
	}

	return allPaths;
}

// GET METHOD ==============================

/**
 * @brief check if the file is a large file
 */
bool Response::isLargeFile(const std::string &path)
{
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) != 0)
	{
		Logger::log(Logger::ERROR, "Failed to stat file: %s", path.c_str());
		return false;
	}
	return fileStat.st_size > THRESHOLD_LARGE_FILE;
}


/**
 * @brief fonction qui determine si il faut
 * renvoyer une page 404 ou 403
 */
void Response::manageNotFound(std::string directoryToCheck)
{
	std::string page;

	if (directoryExist(directoryToCheck.c_str()))
	{
		page = ErrorPage::getPage(403, _blocServer->getErrorPages());
	}
	else
	{
		page = ErrorPage::getPage(404, _blocServer->getErrorPages());
	}

	_response = page;
	setState(Response::FINISH);
}

/**
 * @brief fonctions qui test tout les ficher du vector
 * renvoit un pointeur sur le premier fichier qui existe
 * sinon renvoit NULL
 */
std::string Response::findGoodPath(std::vector<std::string> allPaths)
{
	for (size_t i = 0; i < allPaths.size(); i++)
	{
		Logger::log(Logger::DEBUG, "Trying to open file %s", allPaths[i].c_str());
		if (fileExist(allPaths[i]))
			return allPaths[i];
	}
	return "";
}

void Response::setHeaderChunked(const std::string &path)
{
	_response = "HTTP/1.1 200 OK\r\n";
	_response += "Content-Type: " + getMimeType(path) + "\r\n";
	_response += "Transfer-Encoding: chunked\r\n";
	_response += "\r\n";

	_fileFd = open(path.c_str(), O_RDONLY);
	if (_fileFd == -1)
	{
		Logger::log(Logger::ERROR, "Failed to open file: %s", path.c_str());
		return manageNotFound(path);
	}
	std::cout << "on est rentre dans le header de chunked" << std::endl;
}

/**
 * @brief prepare CHUNKED response
 */
void Response::prepareChunkedResponse(const std::string &path)
{
	if (_state != Response::CHUNK || _fileFd == -1){
		setHeaderChunked(path);
	}

	char buffer[RESPONSE_READ_BUFFER_SIZE];
	ssize_t bytesRead = read(_fileFd, buffer, RESPONSE_READ_BUFFER_SIZE);
	if (bytesRead == -1)
	{
		Logger::log(Logger::ERROR, "Failed to read file: %s", path.c_str());
		manageNotFound(path);
	}
	else if (bytesRead == 0)
	{
		close(_fileFd);
		_fileFd = -1;
		_response += "0\r\n\r\n";
		setState(Response::FINISH);
	}
	else
	{
		Logger::log(Logger::DEBUG, "[ChunkFUNC] Read %ld bytes from file %s", bytesRead, path.c_str());
		std::string chunkSize = intToHexa(bytesRead) + "\r\n";
		std::string chunkData(buffer, bytesRead);
		_response += chunkSize + chunkData + "\r\n";
	}
}


/**
 * @brief prepare STANDARD response with content-length
 * et en lisant en une fois le fichier
 */
void Response::prepareStandardResponse(const std::string &path)
{
	Logger::log(Logger::DEBUG, "[prepareStandardResponse] Opening file %s", path.c_str());
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::stringstream content;
		content << file.rdbuf();
		std::string body = content.str();

		_response = "HTTP/1.1 200 OK\r\n";
		_response += "Content-Type: " + getMimeType(path) + "\r\n";
		_response += "Content-Length: " + intToString(body.size()) + "\r\n";
		_response += "\r\n";
		_response += body;

		file.close();
	}else{
		Logger::log(Logger::ERROR, "Failed to open file: %s", path.c_str());
		manageNotFound(path);
	}
}

/**
 * @brief on check si la requete correspond a un bloc location
 * et on la gere
 */
void Response::manageLocation()
{
	std::string root;
	_blocLocation->getRoot().empty() ? root = _blocServer->getRoot() : root = _blocLocation->getRoot();

	std::vector<std::string> allPathsLocation = getAllPathsLocation();
	std::string path = findGoodPath(allPathsLocation);
	if (path.empty())
		return manageNotFound(root + _request->getUri());

	if (isLargeFile(path)){
		prepareChunkedResponse(path);
		setState(Response::CHUNK);
	}
	else{
		prepareStandardResponse(path);
		setState(Response::FINISH);
	}
}


/**
 * @brief on gere la requete avec le bloc server
 */
void Response::manageServer()
{
	std::vector<std::string> allPathsServer = getAllPathsServer();
	std::string path = findGoodPath(allPathsServer);

	if (path.empty())
		return manageNotFound(_blocServer->getRoot() + _request->getUri());

	if (isLargeFile(path)){
		prepareChunkedResponse(path);
		setState(Response::CHUNK);
	}
	else{
		prepareStandardResponse(path);
		setState(Response::FINISH);
	}
}

/**
 * @brief handle the GET request
 */
void Response::handleGetRequest(void)
{
	if (_blocLocation != NULL)
		manageLocation();
	else
		manageServer();
}

// MAIN RESPONSE ==============================
/**
 * @brief main function to return the response of the request _request
 */
std::string Response::getRawResponse(void)
{
	if (!_response.empty())
		_response.clear();

	//if (_request.getStatusCode() != 200)
	//{
	//	_response = ErrorPage::getPage(_request.getErrorCode(), _blocServer->getErrorPages());
	//	setState(Response::FINISH);
	//	return _response;
	//}

	if (_state != Response::CHUNK)
		setState(Response::PROCESS);

	if (isRedirect()){
		setState(Response::FINISH);
		return _response;
	}
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

void Response::setState(e_response_state state)
{
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