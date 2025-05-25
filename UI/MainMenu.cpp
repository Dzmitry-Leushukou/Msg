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
		std::cout << "===Chats===\n";
		int id = 0;
		for (auto& i : chatHeaders)
		{
			std::cout << id++ << ". " << i << '\n';
		}

	}
}

void MainMenu::getHeaders()
{
	chatHeaders = app->getChatHeaders();
}