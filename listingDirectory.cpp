#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>


/**
 * @brief build the html page with the files in the directory
 */
std::string buildPage(std::vector<std::string> files, std::string path){
	std::string page;
	std::string header = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Listing Directory</title><style>@import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');body{padding: 0;margin: 0;box-sizing: border-box;font-family: 'Inter', sans-serif;background-color: #f9f9f9;}.container{--max-width: 1215px;--padding: 1rem;width: min(var(--max-width), 100% - (var(--padding) * 1.2));margin-inline: auto;}a{list-style-type: none;padding: 0;color: black;}.bigLine{width: 100%;height: 1px;background-color: #e0e0e0;margin: 1rem 0;}ul li{list-style-type: '▪️';padding: .2rem 1rem;margin: 0;}a:visited{color: #9e0999;}</style></head>";
	std::string body = "<body><div class=\"container\"><h1>Index of " + path + "</h1><div class=\"bigLine\"></div><ul>";
	
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
 * @brief List all files in a directory
 * @param path path of the directory
 * 
 */
std::string listDirectory(std::string path){
	std::vector<std::string> files;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return "Error opening directory";
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

	return buildPage(files, path);
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

    if (path.back() != '/')
        path += "/";

    // 1 - Remove all "/./" occurrences
    size_t pos;
    while ((pos = path.find("/./")) != std::string::npos) {
        path.erase(pos, 2); // Supprime le "/." (2 caractères)
    }

		// 2- delete all /prev/../
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
		cleanPath(path);
		std::cout << "Root: " << root << std::endl;
		std::cout << "Path: " << path << std::endl;

		int i = 0;
		while (i < root.size() && i < path.size())
		{
			if (root[i] != path[i])
				break;
			i++;
		}
		return (i == root.size());
}


int main()
{
	std::string root = "/Users/toukoum/42/webserv/www/";
	std::string currentPath = "Users/toukoum/42/webserv/././www/../www/../www/bonjour/ahah/./../";
	if (!is_path_within_root(root, currentPath)) {
		// TODO throw error
		std::cout << "error, path ask is above root" << std::endl;
		return 1;
	}else
		std::cout << "path is clean!" << std::endl;
	std::string page = listDirectory(currentPath);
	std::cout << page << std::endl;
	// create the file index.html
	cleanPath(currentPath);
	std::ofstream file(currentPath);
	file << page;
	file.close();
	return 0;

}