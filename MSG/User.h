#pragma once

#include <string>

class User
{
public:
	User(std::string username) :username(username) { }
	std::string getUsername()const;
private:
	const std::string username;
};

