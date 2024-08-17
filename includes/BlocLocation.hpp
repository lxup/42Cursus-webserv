#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

enum e_Methods
{
	BAD_MET,
	GET,
	POST,
	DELETE
};

enum e_boolMod
{
	FALSE,
	TRUE,
	BAD
};

class BlocLocation
{
	private:
		// config bloc location
		std::string _location;
		std::string _root;
		std::string _rewrite;
		std::string _alias;
		std::vector<std::string> _indexes;
		std::vector<e_Methods> _allowedMethods;
		e_boolMod _autoindex;

		// divers
		std::map<std::string, int> _counterView;
		std::string _filename;
	public:
		BlocLocation(std::string filename);
		~BlocLocation();

		// parsing
		BlocLocation getLocationConfig(std::ifstream &configFile, std::string &path);
		bool isValidLineLocation(std::vector<std::string>& tokens, std::string& key);

		// Methods
		void incrementCounter(const std::string& key) { _counterView[key]++; }
		void checkDoubleLine();
		//void checkValue();
		bool methodsExist();
		e_boolMod strToBool(std::string &str);

		// Adders
		void addAllowedMethods(std::string &token);
		void addFile(std::string &file) { _indexes.push_back(file); }

		// Setters
		void setLocation(const std::string &location) { _location = location; _counterView["location"]++; }
		void setRoot(const std::string &root) { _root = root;   _counterView["root"]++;}
		void setRewrite(const std::string &rewrite) { _rewrite = rewrite;  _counterView["rewrite"]++;}
		void setAlias(const std::string &alias) { _alias = alias;  _counterView["alias"]++;}
		void setFiles(const std::vector<std::string> &files) { _indexes = files;  _counterView["files"]++;}
		void setAllowedMethods(const std::vector<e_Methods> &allowedMethods) { _allowedMethods = allowedMethods;  _counterView["allowedMethods"]++;}
		void setAutoIndex(e_boolMod autoindex) { _autoindex = autoindex;  _counterView["autoindex"]++;}

		// Getters
		const std::string &getLocation() const { return _location; }
		const std::string &getRoot() const { return _root; }
		const std::string &getRewrite() const { return _rewrite; }
		const std::string &getAlias() const { return _alias; }
		const std::vector<std::string> &getFiles() const { return _indexes; }
		const std::vector<e_Methods> &getAllowedMethods() const { return _allowedMethods; }
		e_boolMod getAutoIndex() const { return _autoindex; }

		// Print
		void printLocation(void) const;
};

#endif