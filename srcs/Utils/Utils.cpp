#include "Utils.hpp"


/*
** @brief Create a temporary file
**
** @param path : The path of the temporary file
** @param fd : The file descriptor of the temporary file
** @return 0 if success, -1 otherwise
*/
int	Utils::createTmpFile(std::string &path, int &fd)
{
	// path += "XXXXXX";
	path = "/tmp/webserv_XXXXXX";
	std::vector<char> tmpPath(path.begin(), path.end());
	tmpPath.push_back('\0');

	fd = mkstemp(&tmpPath[0]);
	if (fd == -1)
	{
		Logger::log(Logger::ERROR, "[Utils::createTmpFile] Failed to create temporary file");
		return (-1);
	}
	path.assign(tmpPath.begin(), tmpPath.end() - 1);
	return (0);
}

/*
** @brief Create a temporary file with a random suffix
**
** @param path : The path of the temporary file
** @param fd : The file descriptor of the temporary file
** @return 0 if success, -1 otherwise
*/
int Utils::createFileRandomSuffix(std::string &path, int &fd)
{
	path += "XXXXXX";
	std::vector<char> tmpPath(path.begin(), path.end());
	tmpPath.push_back('\0');

	fd = mkstemp(&tmpPath[0]);
	if (fd == -1)
	{
		Logger::log(Logger::ERROR, "[Utils::createTmpFile] Failed to create temporary file");
		return (-1);
	}
	path.assign(tmpPath.begin(), tmpPath.end() - 1);
	return (0);
}

/*
** @brief Decode a URL-encoded string
**
** @param str : The URL-encoded string
** @return 0 if success, -1 otherwise
*/
int	Utils::urlDecode(std::string &str)
{
	size_t i = 0;
	while (i < str.size())
	{
		if (str[i] == '%')
		{
			if (i + 2 >= str.size())
				return (-1);
			char c = hexToChar(str[i + 1]) * 16 + hexToChar(str[i + 2]);
			str[i] = c;
			str.erase(i + 1, 2);
		}
		i++;
	}
	return (0);
}

/*
** @brief Convert a hexadecimal character to a char
**
** @param c : The hexadecimal character
** @return The char
*/
char	Utils::hexToChar(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	return (0);
}





/* UTILS */

void printMsg(std::ostream &os, const char *msg, ...)
{
	const int initialBufferSize = 1024;
	std::vector<char> buffer(initialBufferSize);

	va_list args;
	va_start(args, msg);
	int size = vsnprintf(buffer.data(), initialBufferSize, msg, args);
	va_end(args);

	// Handling error
	if (size < 0)
	{
		// TODO: Handle error
		return;
	}

	// Resize the buffer if needed
	while (size >= static_cast<int>(buffer.size()))
	{
		buffer.resize(buffer.size() * 2);
		va_start(args, msg);
		size = vsnprintf(buffer.data(), buffer.size(), msg, args);
		va_end(args);
	}

	os << buffer.data() << std::endl;
}

/**
 * @brief renvoie true si le dossier existe
 */
bool directoryExist(const char *path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

/**
 * @brief renvoie true si le fichier existe
 */
bool fileExist(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

/**
 * @brief take string and return trimed line
 * 	example:
 * 		 string => "   bonjour   "
 * 		return : "bonjour"
 * @param line
 * @return std::string
 */
std::string trimLine(std::string &line)
{
	const std::string white_chars = " \t\n\r\f\v";
	std::string result;

	int start = 0;
	while (white_chars.find(line[start]) != std::string::npos)
		start++;
	int end = line.size() - 1;
	while (end >= 0 && white_chars.find(line[end]) != std::string::npos)
		end--;

	for (int i = start; i <= end; i++)
		result.push_back(line[i]);

	return (result);
}

/**
 * @brief Splits a string into multiple substrings based on a delimiter.
 *
 * @example
 * std::string input = "Hello,World,!";
 * std::string delimiter = ",";
 * // result will contain {"Hello", "World", "!"}
 */
std::vector<std::string> split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> result;
	s = trimLine(s);
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
		s = trimLine(s);
	}
	result.push_back(s);
	return (result);
}

std::string unsignedIntToString(unsigned int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

/**
 * @brief Protected call to a function
 * @param ret return code of function to test
 * @param msg message in case of error
 * @param isFatal if true, the error is fatal (optional, default is true)
 * @return the return code of the function if >= 0
 */
int protectedCall(int ret, std::string msg, bool isFatal)
{
	if (ret < 0)
	{
		if (isFatal)
			Logger::log(Logger::FATAL, msg.c_str());
		else
			Logger::log(Logger::ERROR, msg.c_str());
	}
	return ret;
}


std::string		extractIp(std::string ipPort)
{
	return(ipPort.substr(0, ipPort.find(":")));
}

unsigned int	extractPort(std::string ipPort)
{
	return(std::atoi(ipPort.substr(ipPort.find(":") + 1).c_str()));
}
	
/**
 * @brief renvoie true si le fichier est vide
 */
bool isEmptyFile(){
	return (ConfigParser::countLineFile == 0);
}


std::string intToHexa(ssize_t num) {
    std::stringstream stream;
    stream << std::hex << num;
    return stream.str();
}

/**
 * @brief get the extension of a file
 */
std::string getExtension(const std::string &path, bool includeDot)
{
	std::string ext;
	std::string::size_type idx = path.rfind('.');
	if (idx != std::string::npos)
	{
		if (includeDot)
			ext = path.substr(idx);
		else
			ext = path.substr(idx + 1);
	}
	if ((includeDot && ext.size() <= 1) || (!includeDot && ext.empty()))
		return "";
	if (ext.find('/') != std::string::npos)
		return "";
	return ext;
}


// _____________________________ MODIFY EPOLL _____________________________
/**
 * WARNING: this function do not update flags for a socket, it only add socket to 
 * epoll with flags
 */
void addSocketEpoll(int epollFD, int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_ADD, sockFD, &ev), "Error with epoll_ctl function", false);
}

/**
 * @brief
 * WARNING: this function over written previous flags
 * 
 */
void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags)
{
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_MOD, sockFD, &ev), "Error with epoll_ctl function", false);
}

void deleteSocketEpoll(int epollFD, int sockFD)
{
	epoll_event ev;
	ev.data.fd = sockFD;
	protectedCall(epoll_ctl(epollFD, EPOLL_CTL_DEL, sockFD, &ev), "Error with epoll_ctl function", false);
}

std::string getErrorMessage(int code)
{
	switch (code)
	{
		// 2xx Success
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 204:
			return "No Content";
		
		// 3xx Redirection
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		
		// 4xx Client Errors
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Not Allowed";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 413:
			return "Payload Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 429:
			return "Too Many Requests";

		// 5xx Server Errors
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		
		default:
			return "Unknown Error";
	}
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


// _____________________________ LIST DIRECTORY _____________________________



/**
 * @brief build the html page with the files in the directory
 */
std::string buildPage(std::vector<std::string> files, std::string path, std::string root){
	std::string page;
	std::string header = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Listing Directory</title><style>@import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');body{padding: 0;margin: 0;box-sizing: border-box;font-family: 'Inter', sans-serif;background-color: #f9f9f9;}.container{--max-width: 1215px;--padding: 1rem;width: min(var(--max-width), 100% - (var(--padding) * 1.2));margin-inline: auto;}a{list-style-type: none;padding: 0;color: black;}.bigLine{width: 100%;height: 1px;background-color: #e0e0e0;margin: 1rem 0;}ul li{list-style-type: '▪️';padding: .2rem 1rem;margin: 0;}a:visited{color: #9e0999;}</style></head>";
	std::string body = "<body><div class=\"container\"><h1>Index of " + path.substr(root.size()) + "</h1><div class=\"bigLine\"></div><ul>";
	
	// ajoute les lien au body
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		body += "<li><a href=\"";
		body += *it;
		body += "\">";
		body += *it;
		body += "</a></li>";
	}
	body += "</ul><div class=\"bigLine\"></div></div></body></html>";
	
	return header + body;
}



/**
 * @brief fonction qui clean le path
 * ex: /Users/toukoum/42/webserv/././www/../www/../
 * -> /Users/toukoum/42/webserv/
 * 
 * 
 */
void cleanPath(std::string& path){
	 if (path[0] != '/')
        path.insert(0, "/");

    if (path[path.size() - 1] != '/')
        path += "/";

    // Remove all "/./" occurrences
    size_t pos;
    while ((pos = path.find("/./")) != std::string::npos) {
        path.erase(pos, 2); // Supprime le "/." (2 caractères)
    }

		// delete all /prev/../
		while ((pos = path.find("/../")) != std::string::npos){
			if (pos == 0){
				path.erase(0, 3);
				continue;
			}
			size_t prev = path.rfind('/', pos - 1);
			if (prev != std::string::npos){
				path.erase(prev, pos - prev + 3);
			}else{
				path.erase(0, pos + 3);
			}
		}
}

/**
 * @brief fonction qui check si le path demande est un fichier ou un dossier 
 * a l'interieur du dossier root
 * condition:
 * 	root = /42/
 * 	path = /42/toukoum
 * 
 * 
 * @return int 
 */
bool is_path_within_root(const std::string& root, std::string& path) {
		size_t i = 0;
		while (i < root.size() && i < path.size())
		{
			if (root[i] != path[i])
				break;
			i++;
		}
		return (i == root.size());
}


/**
 * @brief List all files in a directory
 * @param path path of the directory
 * 
 */
std::string listDirectory(std::string path, std::string root){

	cleanPath(path);
	if (path[0] != '.')
		path.insert(0, ".");
	Logger::log(Logger::DEBUG, "Root: %s", root.c_str());
	Logger::log(Logger::DEBUG, "Path: %s", path.c_str());

	if (!is_path_within_root(root, path)) {
		Logger::log(Logger::ERROR, "Path asked is not within root");
		return ErrorPage::getPage(403);
	}

	std::vector<std::string> files;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL){
		Logger::log(Logger::ERROR, "Failed to open directory: %s", path.c_str());
		return ErrorPage::getPage(404);
	}
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		files.push_back(ent->d_name);
	}
	closedir(dir);
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
	std::cout << "----------------" << std::endl;

	std::string body = buildPage(files, path, root);
	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: " + intToString(body.size()) + "\r\n";
	header += "\r\n";
	return header + body;
}


std::string uint64ToString(u_int64_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string convertToLowercase(const std::string& str) {
    std::string lowercaseStr = str;
    std::transform(lowercaseStr.begin(), lowercaseStr.end(), lowercaseStr.begin(), ::tolower);
    return lowercaseStr;
}
