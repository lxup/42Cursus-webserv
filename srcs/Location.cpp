/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 10:53:36 by lbehr             #+#    #+#             */
/*   Updated: 2024/08/06 20:16:05 by rgiraud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : _location(""), _root(""), _rewrite(""), _alias(""), _allowedMethods(BAD), _autoindex(FALSE)
{
	(void)_location;
	(void)_root;
	(void)_rewrite;
	(void)_alias;
	(void)_files;
	(void)_allowedMethods;
	(void)_autoindex;
}

Location::~Location()
{
}

void Location::addAllowedMethods(std::string &token)
{
	e_Methods met;
	
	if (token == "GET")
		met = GET;
	else if (token == "POST")
		met = POST;
	else if (token == "DELETE")
		met = DELETE;
	else
		met = BAD_MET;
	_allowedMethods.push_back(met);
}

void Location::printLocation(void) const{
	std::cout << "Location: " << _location << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Rewrite: " << _rewrite << std::endl;
	std::cout << "Alias: " << _alias << std::endl;
	std::cout << "Files: " << std::endl;
	for (std::vector<std::string>::const_iterator it = _files.begin(); it != _files.end(); ++it)
		std::cout << *it << std::endl;
	std::cout << "Allowed methods: " << std::endl;
	for (std::vector<e_Methods>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
		std::cout << *it << std::endl;
	std::cout << "Autoindex: " << _autoindex << std::endl;
}
