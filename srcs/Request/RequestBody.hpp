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

class RequestBody
{
	friend class Request;
	friend class CgiHandler;
	friend class CgiExecutor;
	private:
		std::string		_path;
		// std::ofstream	_stream;
		int				_fd;
		bool			_isTmp;
		u_int64_t		_size;

		/* PRIVATE HELPERS */
		int	_write(const std::string &data);
	public:
		RequestBody(void);
		RequestBody(bool isTmp);
		RequestBody(const RequestBody &src);
		~RequestBody(void);

		RequestBody &operator=(const RequestBody &src);

		/* GETTERS */
		std::string	getPath(void) const { return this->_path; }
		int			getFd(void) const { return this->_fd; }
		bool		isTmp(void) const { return this->_isTmp; }
		u_int64_t	getSize(void) const { return this->_size; }


};

#endif // REQUESTBODY_HPP