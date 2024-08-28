#pragma once
#ifndef CGIHANDLERV2_HPP
# define CGIHANDLERV2_HPP

# include <iostream>

# include "RequestCgi.hpp"

class RequestCgi;

class CgiHandlerV2
{
	friend class RequestCgi;
	private:
		RequestCgi*	_requestCgi;

		/* METHODS */
		void	_init(void);
		void	_execute(void);
	public:
		CgiHandlerV2(RequestCgi* requestCgi);
		CgiHandlerV2(const CgiHandlerV2 &src);
		~CgiHandlerV2(void);

		CgiHandlerV2 &operator=(const CgiHandlerV2 &src);
};

#endif // CGIHANDLERV2_HPP