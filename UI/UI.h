#pragma once

#include <iostream>
#include <clocale>

#include "Chat.h"
#include "User.h"
#include "DBService.h"

class UI
{
public:
	//Constructors
	UI();
	
	//Methods
	void inputHandler();

	//Fields

private:
	//Methods
	void show();
	void login();
	void registration();
	void loadInfo();
	void chatList();
	std::pair<std::string, std::string> userForm();
	
	//Fields
	User* user = nullptr;
	std::vector<std::string>chatHeaders;
	Chat* chat = nullptr;
};

