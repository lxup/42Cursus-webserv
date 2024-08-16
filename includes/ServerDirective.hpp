#pragma once
#ifndef SERVERDIRECTIVE_HPP
# define SERVERDIRECTIVE_HPP

#include <iostream>
#include <vector>

class ServerDirective
{
	private:
		int							_numArg;
		bool						_isDuplicatedAllowed;
		bool						_isBlock;
		bool						(*_checkArgs)(void);
	public:
		ServerDirective(int numArg, bool isDuplicatedAllowed, bool isBlock, bool (*checkArgs)(void));
		~ServerDirective(void);

		/* GETTERS */
		int getNumArg(void) const;
		bool getIsDuplicatedAllowed(void) const;
		bool getIsBlock(void) const;

		/* FUNCTIONS CHECKING ARGUMENTS */
		static bool checkListenDirective(void);
		static bool checkServerNameDirective(void);
		static bool checkServerRootDirective(void);
};


#endif // SERVERDIRECTIVE_HPP