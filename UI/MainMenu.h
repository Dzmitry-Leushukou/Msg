#pragma once
#include "Page.h"
#include "Utils.h"
#include "CreateChatForm.h"
#include <vector>
#include <string>
#include <iostream>
class MainMenu : public Page
{
public:
	MainMenu(Application& app);
	virtual void start() override;
	void getHeaders();
	void inputHandler();
	void help();
private:
	std::vector<std::string>chatHeaders;
};

