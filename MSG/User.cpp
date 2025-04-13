#include "User.h"

User::User(std::string login, std::string password) : name(login)
{
	///genKeys()
}

User::User(std::vector<std::string> data) : name(data.at(0))
{
	///getKeys?
}	

