#include "CgiHandlerV2.hpp"

CgiHandlerV2::CgiHandlerV2(RequestCgi *requestCgi) : _requestCgi(requestCgi) {}

CgiHandlerV2::CgiHandlerV2(const CgiHandlerV2 &src)
{
	*this = src;
}

CgiHandlerV2::~CgiHandlerV2(void) {}

CgiHandlerV2 &CgiHandlerV2::operator=(const CgiHandlerV2 &src)
{
	if (this != &src)
	{
		this->_requestCgi = src._requestCgi;
	}
	return *this;
}

void CgiHandlerV2::_init(void)
{
	// std::cout << "CgiHandlerV2::_init()" << std::endl;
}

void CgiHandlerV2::_execute(void)
{
	// std::cout << "CgiHandlerV2::_execute()" << std::endl;
}