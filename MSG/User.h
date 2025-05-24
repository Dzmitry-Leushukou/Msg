#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User
{
public:
	User(json data);
	void setPublicKey(std::string);
	std::string getPublicKey()const;
private:
	std::string username;
	std::string pub; 
};

