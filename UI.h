#pragma once

#include <iostream>
#include <clocale>

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
	std::pair<std::string, std::string> userForm();
	//Fields
	User* user = nullptr;

};

