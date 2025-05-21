#pragma once
#include "Page.h"
#include <string>
#include "User.h"
class LoginForm:public Page
{
public:
	void start();
	User* getUser()const;
private: 
	void main();
	std::string username();
	std::string password();
	std::string getMethod();
	bool verified(std::string);
	bool exist(std::string);
	bool correct(std::string, std::string);
	User* user=nullptr;
	
};

