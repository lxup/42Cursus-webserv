#pragma once
#ifndef REQUESTBODY_HPP
# define REQUESTBODY_HPP

# include <iostream>
# include <fstream>
# include <cstdio>
# include <cstdlib>
# include <vector>

# include "Utils.hpp"
# include "Logger.hpp"

class Request;

class RequestBody
{
	friend class Request;
	friend class CgiHandler;
	friend class CgiHandlerV2;
	private:
		Request*		_request;
		std::string		_path;
		// std::ofstream	_stream;
		int				_fd;
		bool			_isTmp;
		u_int64_t		_size;

		/* PRIVATE HELPERS */
		int	_write(const std::string &data);
	public:
		RequestBody(void);
		RequestBody(Request* request);
		RequestBody(const RequestBody &src);
		~RequestBody(void);

		RequestBody &operator=(const RequestBody &src);

};

#endif // REQUESTBODY_HPP