#include "ServerDirective.hpp"

ServerDirectiveParserItem::ServerDirectiveParserItem(int numArg, bool isDuplicatedAllowed, bool isBlock, std::string allowedIn, bool (*checkArgs)(void)) : _numArg(numArg), _isDuplicatedAllowed(isDuplicatedAllowed), _isBlock(isBlock), _checkArgs(checkArgs) {
	if (!allowedIn.empty()) {
		std::string directive;
		std::stringstream ss(allowedIn);
		while (std::getline(ss, directive, ','))
			_allowedIn.push_back(directive);
	}
}

ServerDirectiveParserItem::ServerDirectiveParserItem(void) {}

ServerDirectiveParserItem::~ServerDirectiveParserItem(void) {}

int ServerDirectiveParserItem::getNumArg(void) const { return _numArg; }

bool ServerDirectiveParserItem::getIsDuplicatedAllowed(void) const { return _isDuplicatedAllowed; }

bool ServerDirectiveParserItem::getIsBlock(void) const { return _isBlock; }

std::vector<std::string> ServerDirectiveParserItem::getAllowedIn(void) const { return _allowedIn; }

// ServerDirectiveParser
std::map<std::string, ServerDirectiveParserItem> ServerDirectiveParser::serverDirectiveParser = ServerDirectiveParser::_generateServerDirectiveParser();

std::map<std::string, ServerDirectiveParserItem> ServerDirectiveParser::_generateServerDirectiveParser(void)
{
	std::map<std::string, ServerDirectiveParserItem> serverDirectives;

	serverDirectives["server"] = ServerDirectiveParserItem(0, false, true, "", 0);
	serverDirectives["listen"] = ServerDirectiveParserItem(1, false, false, "server", 0);
	serverDirectives["server_name"] = ServerDirectiveParserItem(1, false, false, "server", 0);
	serverDirectives["location"] = ServerDirectiveParserItem(0, false, true, "server", 0);

	// print serverDirectives
	for (std::map<std::string, ServerDirectiveParserItem>::iterator it = serverDirectives.begin(); it != serverDirectives.end(); it++)
	{
		std::cout << it->first << " : " << it->second.getNumArg() << " " << it->second.getIsDuplicatedAllowed() << " " << it->second.getIsBlock() << std::endl;
		std::vector<std::string> allowedIn = it->second.getAllowedIn();
		for (size_t i = 0; i < allowedIn.size(); i++)
			std::cout << allowedIn[i] << " " << std::endl;
	}
	
	return serverDirectives;
}

ServerDirective::ServerDirective(std::string name, std::vector<std::string> args, bool isBlock) : _name(name), _args(args), _isBlock(isBlock) {}

ServerDirective::~ServerDirective(void) {}
