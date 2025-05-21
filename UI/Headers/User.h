#pragma once

#include <string>
#include <vector>

#include "Client.h"

class User
{
public:
	//Constructors
	User(std::string,std::string);
	User(std::string,std::vector<std::string>);

	std::vector<std::string>getChatsId()const;
private:
	const std::string name;
	std::vector<std::string>chatsId;
	
};

