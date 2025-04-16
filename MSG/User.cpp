#include "User.h"

User::User(std::string login, std::string password) : name(login)
{
	///genKeys()
}

User::User(std::string login, std::vector<std::string> data) : name(login)
{
	///getKeys?
	chatsId = data;
}	

std::vector<std::string> User::getChatsId() const
{
	return chatsId;
}

