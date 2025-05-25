#pragma once
#include "Page.h"
#include <vector>
#include <string>
class MainMenu : public Page
{
public:
	MainMenu(Application& app);
	virtual void start() override;
	void getHeaders();
private:
	std::vector<std::string>chatHeaders;
};

