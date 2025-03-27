#pragma once

#include <string>
#include <stdexcept>

#include "User.h"

static class DBService
{
public:
	static void checkUser(std::string username, std::string password)
	{
		throw std::invalid_argument("");
	}
	static std::string getUser(std::string username, std::string password)
	{
		return "";
	}
	static void createUser(User* user)
	{
		return ;
	}
};

