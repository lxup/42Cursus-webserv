#pragma once
#ifndef SERVERDIRECTIVEPARSER_HPP
# define SERVERDIRECTIVEPARSER_HPP

#include <iostream>
#include <vector>

class ServerDirectiveParser
{
	private:
		int							_numArg;
		bool						_isDuplicatedAllowed;
		bool						_isBlock;
		bool						(*_checkArgs)(void);
	public:
		ServerDirectiveParser(int numArg, bool isDuplicatedAllowed, bool isBlock, bool (*checkArgs)(void));
		~ServerDirectiveParser(void);

		/* GETTERS */
		int getNumArg(void) const;
		bool getIsDuplicatedAllowed(void) const;
		bool getIsBlock(void) const;

		/* FUNCTIONS CHECKING ARGUMENTS */
		static bool checkListenDirective(void);
		static bool checkServerNameDirective(void);
		static bool checkServerRootDirective(void);
};


#endif // SERVERDIRECTIVEPARSER_HPP