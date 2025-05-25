#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User
{
public:
	User(std::string username) :username(username) { }

private:
	const std::string username;
};

