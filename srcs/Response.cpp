#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

// Response::Response() : _request(NULL), _blocServer(NULL), _blocLocation(NULL), _state(INIT), _fileFd(-1)
// {
// }

Response::Response(Client* client) : _client(client), _request(client->getRequest()), _state(Response::INIT), _fileFd(-1)
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
	std::string path = _request->getPath();
	std::string root;
	bool isLoc;

	isLoc = this->_request->getLocation() == NULL ? false : true;

	if (isLoc && !this->_request->getLocation()->getRewrite().second.empty())
	{
		std::pair<int, std::string> rewrite = this->_request->getLocation()->getRewrite();
		_response = "HTTP/1.1 " + intToString(rewrite.first) + " " + getErrorMessage(rewrite.first) + "\r\n";
		_response += "Location:" + rewrite.second + "\r\n";
		_response += "Content-Length: 0\r\n";
		_response += "\r\n";
		return (true);
	}

	if (isLoc && !this->_request->getLocation()->getRoot().empty())
		root = this->_request->getLocation()->getRoot();
	else
		root = this->_request->getServer()->getRoot();

	// si l'uri fini par / ou est egal a / on ne redirige pas
	if (path[path.size() - 1] == '/' || path == "/")
	{
		return false;
	}

	if (directoryExist((root + path).c_str()) || (isLoc && directoryExist((this->_request->getLocation()->getAlias() + path.substr(this->_request->getLocation()->getPath().size())).c_str())))
	{
		std::string host = _request->getHeaders()["Host"];
		_response = "HTTP/1.1 301 Moved Permanently\r\n";
		_response += "Location: http://" + host + path + "/\r\n";
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
	std::string path = _request->getPath();
	std::string root = this->_request->getLocation()->getRoot();
	std::string alias = this->_request->getLocation()->getAlias();
	std::vector<std::string> indexes = this->_request->getLocation()->getIndexes();
	bool isAlias = false;

	if (this->_request->getLocation() == NULL)
		return std::vector<std::string>();

	if (root.empty())
		root = this->_request->getServer()->getRoot();

	if (!alias.empty())
	{
		root = alias;
		isAlias = true;
	}

	// cas ou la requete demande un fichier direct
	if (path[path.size() - 1] != '/'){
		if (isAlias)
			path = path.substr(this->_request->getLocation()->getPath().size());
		allPathsLocations.push_back(root + path);
	}

	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string index = indexes[i];
		if (path == "/"){
			path = root + "/" + index;
		}
		else if (isAlias){
			path = root + "/" + index;
		}
		else{
			path = root + path + index;
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
	std::string path = _request->getPath();
	std::string root = this->_request->getServer()->getRoot();
	std::vector<std::string> indexes = this->_request->getServer()->getIndexes();
	std::vector<std::string> allPaths;

	// cas ou la requete demande un fichier direct
	if (path[path.size() - 1] != '/')
		allPaths.push_back(root + path);

	// cas ou la requete demande un dossier
	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string index = indexes[i];

		if (path == "/")
		{
			path = root + "/" + index;
		}
		else
		{
			path = root + path + index;
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
		page = ErrorPage::getPage(403, this->_request->getServer()->getErrorPages());
	}
	else
	{
		page = ErrorPage::getPage(404, this->_request->getServer()->getErrorPages());
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
	this->_request->getLocation()->getRoot().empty() ? root = this->_request->getServer()->getRoot() : root = this->_request->getLocation()->getRoot();

	std::vector<std::string> allPathsLocation = getAllPathsLocation();
	std::string path = findGoodPath(allPathsLocation);

	if (path.empty()){
		if (this->_request->getLocation()->getAutoIndex() == TRUE){
			std::string alias = this->_request->getLocation()->getAlias();
			if (!alias.empty()){
				// transforme le path en path court: /var/www/html/ -> /www/html/
				std::string shortPath = _request->getPath().substr(_request->getLocation()->getPath().size());
				_response = listDirectory(alias + shortPath, alias);
			}
			else
				_response = listDirectory(root + _request->getPath(), root);
			setState(Response::FINISH);
			return ;
		}
		return manageNotFound(root + _request->getPath());
	}

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
		return manageNotFound(this->_request->getServer()->getRoot() + _request->getPath());

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
	// if (_blocLocation != NULL)
	if (this->_request->getLocation() != NULL)
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

	if (_request->getStatusCode() != REQUEST_DEFAULT_STATE_CODE)
	{
		_response = ErrorPage::getPage(_request->getStatusCode(), this->_request->getServer()->getErrorPages());
		setState(Response::FINISH);
		return _response;
	}

	if (_state != Response::CHUNK)
		setState(Response::PROCESS);

	if (isRedirect()){
		setState(Response::FINISH);
		return _response;
	}
	if (_request->getMethod() == "GET"){
		handleGetRequest();
	}
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

/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the CGI response
 * 
 */
//int Response::handleCGIResponse(int epollFD)
//{
//	if (pipe(this->_cgiHandler.getPipe()) == -1)
//	{
//		Logger::log(Logger::ERROR, "Failed to create pipe");
//		return (-1);
//	}
//	if (this->_cgiHandler.init(this->_request))
//	{
//		Logger::log(Logger::ERROR, "Failed to init CGI handler");
//		return (-1);
//	}
//	if (this->_cgiHandler.execute())
//	{
//		Logger::log(Logger::ERROR, "Failed to execute CGI handler");
//		return (-1);
//	}
//	return (0);
//}