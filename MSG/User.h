#pragma once

#include <string>

class User
{
public:
	User(std::string);
	void setPublicKey(std::string);
	std::string getPublicKey()const;
private:
	const std::string username;
	std::string pub; 
};

