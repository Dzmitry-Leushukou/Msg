#include "User.h"

User::User(std::string data)
{
	username = data;
	loadKeys();
}

User::User(std::string username, std::string password, std::vector<std::string> dk, std::vector<std::string> ek)
{
	this->username = username;
	this->password = password;	
	decryption_keys = dk;
	encryption_keys = ek;
}

void User::loadKeys()
{	
	std::ifstream fin(usernameToId() + ".tmp");

	std::string s;
	
	if (fin >> s&&s != "=" + usernameToId() + "=")
	{
		return;
	}

	while (fin >> s&& s != "=" + usernameToId() + "=")
	{
		decryption_keys.push_back(s);
	}

	while (fin >> s)
	{
		encryption_keys.push_back(s);
	}

}

std::string User::usernameToId() const
{
	std::string key;
	for (auto& i : username)
	{
		key += std::to_string(abs((i * (i - 1) * (i - 2)) % ((int)1e9 + 7)));
	}

	return key;
}

std::vector<std::string> User::getListOfChats()
{
	//return DBService::getChats(username);
	return {};
}