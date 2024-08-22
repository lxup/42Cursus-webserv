#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
# include <iomanip>

#include "Utils.hpp"
#include "Logger.hpp"
# include "ConfigParser.hpp"

enum e_Methods
{
	GET,
	POST,
	DELETE,
	UNKNOWN
};

enum e_boolMod
{
	FALSE,
	TRUE
};

class BlocLocation
{
	private:
		std::string	_path;
		std::string _root;
		std::pair<int, std::string> _rewrite;
		std::string _alias;
		std::vector<std::string> _indexes;
		std::vector<e_Methods> _allowedMethods;
		e_boolMod _autoindex;
		std::map<std::string, std::string> _cgiExtension;
		std::string _uploadPath;

		// divers
		std::map<std::string, int> _counterView;
		std::string _filename;

	public:
		BlocLocation(std::string filename);
		BlocLocation(const BlocLocation &other);
		~BlocLocation();

		BlocLocation &operator=(const BlocLocation &other);

		// parsing
		BlocLocation getLocationConfig(std::ifstream &configFile, std::string &path);
		bool isValidLineLocation(std::vector<std::string>& tokens, std::string& key);

		// Methods
		void incrementCounter(const std::string& key) { _counterView[key]++; }
		void checkDoubleLine();
		void setDefaultValues();
		e_boolMod strToBool(std::string &str);
		void cleanPaths();

		// Adders
		void addAllowedMethods(std::vector<std::string> &tokens);
		void addIndexes(std::vector<std::string>& token);
		void addCgiExtension(std::vector<std::string>& token);

		// Setters
		void setPath(const std::string &path) { _path = path; }
		void setUploadPath(const std::string &uploadPath) { _uploadPath = uploadPath; _counterView["upload_path"]++;}
		void setRoot(const std::string &root) { _root = root;  _counterView["root"]++;}
		void setRewrite(std::vector<std::string>& tokens);
		void setAlias(const std::string &alias) { _alias = alias;  _counterView["alias"]++;}
		void setAutoIndex(e_boolMod autoindex) { _autoindex = autoindex;  _counterView["autoindex"]++;}

		// Getters
		const std::string &getPath() const { return _path; }
		const std::string &getRoot() const { return _root; }
		const std::pair<int, std::string> &getRewrite() const { return _rewrite; }
		const std::string &getAlias() const { return _alias; }
		const std::vector<std::string> &getFiles() const { return _indexes; }
		const std::vector<e_Methods> &getAllowedMethods() const { return _allowedMethods; }
		e_boolMod getAutoIndex() const { return _autoindex; }
		const std::vector<std::string> &getIndexes() const { return _indexes; }
		// Print
		void printLocation(void);
		void printPair(const std::string& label, const std::string& value);
		void printBool(const std::string& label, bool value, const std::string& trueStr, const std::string& falseStr);
		void printVector(const std::string& label, const std::vector<std::string>& vec);
		void printMap(const std::string& label, const std::map<std::string, std::string>& map);

		// Is
		bool	isMethodAllowed(e_Methods method);

		/* UTILS */
		static e_Methods	converStrToMethod(const std::string &method);
};

#endif