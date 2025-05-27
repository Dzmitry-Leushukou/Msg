#include "MainMenu.h"

MainMenu::MainMenu(Application& app)
{
	this->app = &app;
	getHeaders();
}
void MainMenu::start()
{
	while (app->authorized())
	{
		clearScreen();
		
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
		std::cout << "Creating chat...";
		app->createChat(cc->getName());
		delete cc;
		cc = nullptr;
		clearScreen();
		std::cout << "Updating info..";
		getHeaders();
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
		InvitesMenu* rm = new InvitesMenu(*app);
		rm->start();
		delete rm;
		rm = nullptr;
	}
	else
	if (s == "d")
	{
		clearScreen();
		std::cout << "Clean your soul. Just wait :)\n";
		app->deleteUser();
	}
	else
		if (s == "q")
		{
			clearScreen();
			app->clearUser();
			return;
		}
		else
	if (Utils::isUInt(s)&&stoul(s)<chatHeaders.size())
	{
		ChatUI* c = new ChatUI(*app,stoul(s));
		c->start();
		delete c;
		c = nullptr;
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
	std::cout << "q | Go to login page\n";
	std::cout << "--------------------------------\n";
}

void MainMenu::getHeaders()
{
	chatHeaders = app->getChatHeaders();
}