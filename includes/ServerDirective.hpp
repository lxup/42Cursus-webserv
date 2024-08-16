#pragma once
#ifndef SERVERDIRECTIVEPARSER_HPP
# define SERVERDIRECTIVEPARSER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

class ServerDirectiveParserItem
{
	private:
		int							_numArg;
		bool						_isDuplicatedAllowed;
		bool						_isBlock;
		std::vector<std::string>	_allowedIn;
		bool						(*_checkArgs)(void);
		
	public:
		ServerDirectiveParserItem(void);
		ServerDirectiveParserItem(int numArg, bool isDuplicatedAllowed, bool isBlock, std::string allowedIn, bool (*checkArgs)(void));
		~ServerDirectiveParserItem(void);

		/* GETTERS */
		int getNumArg(void) const;
		bool getIsDuplicatedAllowed(void) const;
		bool getIsBlock(void) const;
		std::vector<std::string> getAllowedIn(void) const;

		/* FUNCTIONS CHECKING ARGUMENTS */
		static bool checkListenDirective(void);
		static bool checkServerNameDirective(void);
		static bool checkServerRootDirective(void);
};

class ServerDirectiveParser
{
	private:
		static std::map<std::string, ServerDirectiveParserItem> _generateServerDirectiveParser(void);
	public:
		static std::map<std::string, ServerDirectiveParserItem> serverDirectiveParser;
};

class ServerDirective
{
	private:
		std::string						_name;
		std::vector<std::string>		_args;
		std::vector<ServerDirective>	_children;
		bool							_isBlock;
	public:
		ServerDirective(std::string name, std::vector<std::string> args, bool isBlock);
		~ServerDirective(void);



		/* GETTERS */

};

#endif // SERVERDIRECTIVEPARSER_HPP