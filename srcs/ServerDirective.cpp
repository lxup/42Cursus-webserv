#include "ServerDirective.hpp"

/*
* Server Directives :
* 	- num arg : number of arguments and -1 if unlimited
* 	- is duplicated allowed : if the directive can be duplicated
* 	- is block : if the directive is a block
*/
ServerDirective::ServerDirective(int numArg, bool isDuplicatedAllowed, bool isBlock, bool (*checkArgs)(void)) : _numArg(numArg), _isDuplicatedAllowed(isDuplicatedAllowed), _isBlock(isBlock), _checkArgs(checkArgs) {}

ServerDirective::~ServerDirective(void) {}

int ServerDirective::getNumArg(void) const { return _numArg; }

bool ServerDirective::getIsDuplicatedAllowed(void) const { return _isDuplicatedAllowed; }

bool ServerDirective::getIsBlock(void) const { return _isBlock; }
