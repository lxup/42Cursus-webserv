#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

// Response::Response() : _request(NULL), _blocServer(NULL), _blocLocation(NULL), _state(INIT), _fileFd(-1)
// {
// }

Response::Response(Client* client) : _request(client->getRequest()), _cgiHandler(this), _state(Response::INIT), _fileFd(-1)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response()
{
	if (_fileFd != -1)
		close(_fileFd);
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

	// return redirection
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

/*
* @brief handle the POST request
*/
void Response::handlePostRequest(void)
{
	std::string jsonBody = "{\n";
	jsonBody += "\"message\": \"File uploaded successfully.\",\n";
	jsonBody += "\"filename\": \"" + _request->_body.getPath() + "\",\n";
	jsonBody += "\"size\": " + uint64ToString(_request->_body.getSize()) + "\n";
	jsonBody += "}\n";

	_response = "HTTP/1.1 200 OK\r\n";
	_response += "Content-Type: application/json\r\n";
	_response += "Content-Length: " + intToString(jsonBody.size()) + "\r\n";
	_response += "\r\n";
	_response += jsonBody;
	this->setState(Response::FINISH);
}

/*
* @brief handle the DELETE request
*/
void Response::handleDeleteRequest(void)
{
	std::string path = this->_request->_location ? this->_request->_location->getRoot() + this->_request->getPath() : this->_request->getServer()->getRoot() + this->_request->getPath();
	if (!fileExist(path))
		return (this->setError(404));
	if (directoryExist(path.c_str()) || remove(path.c_str()) != 0)
		return (this->setError(403));
	std::string jsonBody = "{\n";
	jsonBody += "\"message\": \"File deleted successfully.\",\n";
	jsonBody += "\"filename\": \"" + _request->getPath() + "\"\n";
	jsonBody += "}\n";

	_response = "HTTP/1.1 200 OK\r\n";
	_response += "Content-Type: application/json\r\n";
	_response += "Content-Length: " + intToString(jsonBody.size()) + "\r\n";
	_response += "\r\n";
	_response += jsonBody;
	this->setState(Response::FINISH);
}

/*
* @brief handle the PUT request
*/
void Response::handlePutRequest(void)
{
	std::string jsonBody = "{\n";
	jsonBody += "\"message\": \"File uploaded successfully.\",\n";
	jsonBody += "\"filename\": \"" + _request->_body.getPath() + "\",\n";
	jsonBody += "\"size\": " + uint64ToString(_request->_body.getSize()) + "\n";
	jsonBody += "}\n";

	_response = "HTTP/1.1 200 OK\r\n";
	_response += "Content-Type: application/json\r\n";
	_response += "Content-Length: " + intToString(jsonBody.size()) + "\r\n";
	_response += "\r\n";
	_response += jsonBody;
	this->setState(Response::FINISH);
}

// MAIN RESPONSE ==============================
/**
 * @brief main function to return the response of the request _request
 * 
 * @param epollFD
 * @return int : 0 if the response need to be send, -1 if the response is not ready
 */
int Response::generateResponse(int epollFD)
{
	(void)epollFD;
	if (!_response.empty())
		_response.clear();

	if (_request->getStateCode() != REQUEST_DEFAULT_STATE_CODE)
		return (this->setError(_request->getStateCode()), 0);


	if (isRedirect())
		return (this->setState(Response::FINISH), 0);

	if (this->_request->isCgi())
	{
		Logger::log(Logger::DEBUG, "ITS A CGI");
		return (this->_handleCgi());
	}
	Logger::log(Logger::DEBUG, "ITS NOT A CGI");


	if (_state != Response::CHUNK)
		this->setState(Response::PROCESS);
	// if (this->isCGI())
	// {
	// 	Logger::log(Logger::DEBUG, "ITS A CGI");
	// 	this->handleCGI(epollFD);
	// 	return ;
	// }

	if (_request->getMethod() == "GET")
		handleGetRequest();
	else if (_request->getMethod() == "POST")
		handlePostRequest();
	else if (_request->getMethod() == "DELETE")
		handleDeleteRequest();
	else if (_request->getMethod() == "PUT")
		handlePutRequest();
	// else if (_request->getMethod() == "PUT")
	// 	return (this->setError(405), 0);
	else
		return (this->setError(405), 0);
	
	return (0);
		// this->_response = "HTTP/1.1 405 Method Not Allowed\r\n";
	// else if (_request.getMethod == "POST")
	//     handlePostRequest();
	// else if (_request.getMethod == "DELETE")
	//     handleDeleteRequest();
	// else if (_request.getMethod == "PUT")
	//     handlePutRequest();
	// else
	//     handleOtherRequest();
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
** --------------------------------- SETTERS ---------------------------------
*/

void	Response::setError(int code, bool generatePage)
{
	this->_request->setStateCode(code);
	if (generatePage)
		this->_response = ErrorPage::getPage(code, this->_request->getServer()->getErrorPages());
	this->setState(Response::FINISH);
}


/*
** --------------------------------- HANDLE ---------------------------------
*/

/**
 * @brief Handle the CGI response
 * 
 * @return int : 0 if the response need to be send, -1 if the response is not ready
 */
int Response::_handleCgi(void)
{
	Logger::log(Logger::DEBUG, "[Reponse::_handleCgi] Handling CGI response");
	if (this->_state == Response::FINISH)
		return (-1);
	if (this->_state == Response::INIT)
	{
		if (lseek(this->_request->_cgi._cgiHandler->getFdOut(), 0, SEEK_SET) == -1)
			return (this->setError(500), 0);
		this->setState(Response::PROCESS);
	}
	char buffer[RESPONSE_READ_BUFFER_SIZE] = {0};

	memset(buffer, 0, RESPONSE_READ_BUFFER_SIZE);
	ssize_t bytesRead = read(this->_request->_cgi._cgiHandler->getFdOut(), buffer, RESPONSE_READ_BUFFER_SIZE - 1);
	if (bytesRead == -1)
		throw IntException(500);
	if (bytesRead == 0)
	{
		Logger::log(Logger::DEBUG, "[Reponse::_handleCgi] No more data to read");
		if (this->_cgiHandler._isChunked)
			this->_response += "0\r\n\r\n";
		return (this->setState(Response::FINISH), 0);
	}
	buffer[bytesRead] = '\0';
	std::string str(buffer, bytesRead);
	this->_cgiHandler._parse(str);
	// std::cout << C_RED << "BUFFER: " << str << C_RESET << std::endl;
	// exit(0);
	if (this->_response.empty())
		return (-1);
	return (0);
}
