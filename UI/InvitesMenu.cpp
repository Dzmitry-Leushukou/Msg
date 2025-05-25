#include "InvitesMenu.h"

InvitesMenu::InvitesMenu(Application& app)
{
	this->app = &app;
}

void InvitesMenu::start()
{
	clearScreen();
	while (true)
	{
		std::string s;
		clearScreen();
		std::cout << "[Invite]\n";
		
		std::pair<std::string, std::string>q = app->getInvite();
		if (q.first == "")
			std::cout << "No invites\n";
		else
			std::cout << "Invite to chat \""+app->getChatName(q.first)+"+\" from " + q.second << '\n';
		std::cout << "-------------------------------\n";
		help();
		std::getline(std::cin, s);
		if (s == "q")
			return;
		if (s == "y")
		{
			if (q.first == "")
				app->acceptInvite(q.first);
		}
		else if (s == "n")
		{

		}
		else
		{
			std::cout << "Wrong input\n";
			pause();
			continue;
		}
		app->popInvite();
	}
}

void InvitesMenu::help()
{
	std::cout << "Command | Description\n";
	std::cout << "q | quit\n";
	std::cout << "y | accept\n";
	std::cout << "n | decline\n";
}