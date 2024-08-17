#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

class Request
{
	private:
		std::string _method;
		std::string _uri;
		std::string _httpVersion;
		std::string _body;
		std::string	_version;
		std::map<std::string, std::string> _headers;
	
	public:
		Request(void);
		Request(const Request &src);
		~Request(void);

		Request &operator=(const Request &rhs);

		/* GETTERS */
		std::string getMethod(void) const { return _method; }
		std::string getUri(void) const { return _uri; }
		std::string getHttpVersion(void) const { return _httpVersion; }
		std::string getBody(void) const { return _body; }
		std::string getVersion(void) const { return _version; }
		std::map<std::string, std::string> getHeaders(void) const { return _headers; }

		/* SETTERS */
		void setMethod(std::string method) { _method = method; }
		void setUri(std::string uri) { _uri = uri; }
		void setHttpVersion(std::string httpVersion) { _httpVersion = httpVersion; }
		void setBody(std::string body) { _body = body; }
		void setVersion(std::string version) { _version = version; }
		void setHeaders(std::map<std::string, std::string> headers) { _headers = headers; }
};

#endif // REQUEST_HPP