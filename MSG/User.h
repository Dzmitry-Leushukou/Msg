#pragma once

#include <string>
#include <vector>
#include <fstream>

class User
{
public:
	//Constructors
	User(std::string);
	User(std::string, std::string, std::vector<std::string> dk = {}, std::vector<std::string> ek = {});
	//Methods
	std::vector<std::string> getListOfChats();
	//Fields

private:
	//Methods
	std::string usernameToId() const;
	void loadKeys();
	//Fields
	std::string username;
	std::string password;
	std::vector<std::string>decryption_keys,encryption_keys;
};

