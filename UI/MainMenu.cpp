#include "MainMenu.h"

MainMenu::MainMenu(Application& app)
{
	this->app = &app;
}
void MainMenu::start()
{
	while (app->authorized())
	{
		clearScreen();
		getHeaders();
		std::cout << "===Chats===\n";
		unsigned int id = 0;
		for (auto& i : chatHeaders)
		{
			std::cout << id++ << ". " << i << '\n';
		}
		std::cout << "--------------------------------\n";
		help();
		inputHandler();
	}
}

void MainMenu::inputHandler()
{
	std::string s;
	std::getline(std::cin, s);
	if (s == "c")
	{
		CreateChatForm* cc = new CreateChatForm(*app);
		cc->start();
		if (cc->getName() == "")
			return;
		app->createChat(cc->getName());
		delete cc;
		cc = nullptr;
;	}
	else
	if (s == "r")
	{
		RequestsMenu* rm = new RequestsMenu(*app);
		rm->start();
		delete rm;
		rm = nullptr;
	}
	else
	if (s == "i")
	{

	}
	else
	if (s == "d")
	{
		clearScreen();
		std::cout << "Clean your soul. Just wait :)\n";
		app->deleteUser();
	}
	else
	if (Utils::isUInt(s))
	{
		
	}
	else
	{
		std::cout << "Wrong input. Try again\n";
		system("pause");
	}
}

void MainMenu::help()
{
	std::cout << "Command | Description\n";
	std::cout << "c | Create chat\n";
	std::cout << "r | Show MAC requests\n";
	std::cout << "i | Show chat invites\n";
	std::cout << "id | Open chat by id\n";
	std::cout << "d | Delete account\n";
	std::cout << "--------------------------------\n";
}

void MainMenu::getHeaders()
{
	chatHeaders = app->getChatHeaders();
}