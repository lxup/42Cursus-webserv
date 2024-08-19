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
std::string buildPage(std::vector<std::string> files){
	std::string page = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><ul>";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		page += "<li><a href=\"";
		page += *it;
		page += "\">";
		page += *it;
		page += "</a></li>";
	}
	page += "</ul></body></html>";
	return page;
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

	return buildPage(files);
}

/**
 * @brief fonction qui check si le path demande est un fichier ou un dossier 
 * a l'interieur du dossier root
 * 
 * @return int 
 */
bool is_path_within_root(const std::string& root, const std::string& path) {
    std::filesystem::absolute root_path = std::filesystem::canonical(root);
    std::filesystem::absolute requested_path = std::filesystem::canonical(path);

    return requested_path.string().find(root_path.string()) == 0;
}


int main()
{
	std::string root = "/Users/toukoum/42/webserv/www/";
	std::string currentPath = "/Users/toukoum/42/webserv/www/toukoum";
	if (!is_path_within_root(root, currentPath)) {
		// thorw error
		std::cout << "error" << std::endl;
		return 1;
	}
	std::string page = listDirectory(currentPath);
	std::cout << page << std::endl;
	// create the file index.html
	std::ofstream file("/Users/toukoum/42/webserv/www/toukoum/index.html");
	file << page;
	file.close();
	return 0;

}