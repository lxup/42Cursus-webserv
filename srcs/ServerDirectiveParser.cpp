#include "ServerDirectiveParser.hpp"

/*
* Server Directives :
* 	- num arg : number of arguments and -1 if unlimited
* 	- is duplicated allowed : if the directive can be duplicated
* 	- is block : if the directive is a block
*/
ServerDirectiveParser::ServerDirectiveParser(int numArg, bool isDuplicatedAllowed, bool isBlock, bool (*checkArgs)(void)) : _numArg(numArg), _isDuplicatedAllowed(isDuplicatedAllowed), _isBlock(isBlock), _checkArgs(checkArgs) {}

ServerDirectiveParser::~ServerDirectiveParser(void) {}

int ServerDirectiveParser::getNumArg(void) const { return _numArg; }

bool ServerDirectiveParser::getIsDuplicatedAllowed(void) const { return _isDuplicatedAllowed; }

bool ServerDirectiveParser::getIsBlock(void) const { return _isBlock; }
