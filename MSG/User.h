#pragma once

#include <string>
#include <vector>

#include "Client.h"

class User
{
public:
	//Constructors
	User(std::string,std::string);
	User(std::vector<std::string> data);
private:
	const std::string name;
};

