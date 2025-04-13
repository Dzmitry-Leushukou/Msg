#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "User.h"

static class DBService
{
public:
	static void checkUser(std::string username, std::string password) //Exist or not
	{
		throw std::invalid_argument("");
	}
	static std::string getUser(std::string username, std::string password) //Get user fields
	{
		return "";
	}
	static void createUser(User * user)
	{
		return ;
	}
	static std::vector < std::string> getChats(std::string username)
	{
		return {};
	}
};

