#pragma once
#include "Page.h"
#include "Application.h"
#include <string>
#include <vector>
class LoginForm:public Page
{
public:
	LoginForm(Application& app);
	virtual void start() override;
private: 
	void main();
	void signView(std::vector<std::string>);
	std::string username();
	std::string password();
	std::string getMethod();
	bool verified(std::string);
	bool exist(std::string);
	bool correct(std::string, std::string);	

	Application* app;
};

