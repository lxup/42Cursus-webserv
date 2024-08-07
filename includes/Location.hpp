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

class Location
{
	private:
		std::string _location;
		std::string _root;
		std::string _rewrite;
		std::string _alias;
		std::vector<std::string> _files;
		std::vector<e_Methods> _allowedMethods;
		e_boolMod _autoindex;
		std::map<std::string, int> _counter;

	public:
		Location();
		~Location();

		// Methods
		void incrementCounter(const std::string& key) { _counter[key]++; }
		void checkDoubleLine();
		bool checkValue();
		bool fileExistVector();
		bool methodsExist();

		// Adders
		void addAllowedMethods(std::string &token);
		void addFile(std::string &file) { _files.push_back(file); }

		// Setters
		void setLocation(const std::string &location) { _location = location; _counter["location"]++; }
		void setRoot(const std::string &root) { _root = root;   _counter["root"]++;}
		void setRewrite(const std::string &rewrite) { _rewrite = rewrite;  _counter["rewrite"]++;}
		void setAlias(const std::string &alias) { _alias = alias;  _counter["alias"]++;}
		void setFiles(const std::vector<std::string> &files) { _files = files;  _counter["files"]++;}
		void setAllowedMethods(const std::vector<e_Methods> &allowedMethods) { _allowedMethods = allowedMethods;  _counter["allowedMethods"]++;}
		void setAutoIndex(e_boolMod autoindex) { _autoindex = autoindex;  _counter["autoindex"]++;}

		// Getters
		const std::string &getLocation() const { return _location; }
		const std::string &getRoot() const { return _root; }
		const std::string &getRewrite() const { return _rewrite; }
		const std::string &getAlias() const { return _alias; }
		const std::vector<std::string> &getFiles() const { return _files; }
		const std::vector<e_Methods> &getAllowedMethods() const { return _allowedMethods; }
		e_boolMod getAutoIndex() const { return _autoindex; }

		// Print
		void printLocation(void) const;
};

#endif