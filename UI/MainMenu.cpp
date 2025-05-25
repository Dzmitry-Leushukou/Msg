#include "MainMenu.h"

MainMenu::MainMenu(Application& app)
{
	this->app = &app;
}
void MainMenu::start()
{
	while (true)
	{
		clearScreen();
		getHeaders();
	}
}

void MainMenu::getHeaders()
{
	chatHeaders = app->getChatHeaders();
}