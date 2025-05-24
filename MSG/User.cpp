#include "User.h"

User::User(json data)
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