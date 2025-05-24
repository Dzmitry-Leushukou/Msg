#include "User.h"

User::User(std::string login) : username(login)
{
	
}
void User::setPublicKey(std::string public_key)
{
	this->pub = public_key;
}

std::string User::getPublicKey() const
{
	return pub;
}