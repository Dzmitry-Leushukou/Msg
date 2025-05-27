#include "ChatUI.h"

ChatUI::ChatUI(Application& app, unsigned int num)
{
	this->app = &app;
	//app->getChat(num);
}

void ChatUI::start()
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
			std::cout << "Invite to chat \"" + app->getChatName(q.first) + "+\" from " + q.second << '\n';
		std::cout << "-------------------------------\n";
		help();
		std::getline(std::cin, s);
		if (s == "q")
			return;
		if (s == "y")
		{
			if (q.first != "")
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

void ChatUI::help()
{
	std::cout << "Command | Description\n";
	std::cout << "/q | quit\n";
	std::cout << "/h | help\n";
	std::cout << "/img | to choose message to save like image\n";
	std::cout << "/i | to invite user to the chat (this operation need MUCH TIME)\n";
	std::cout << "/d | to delete chat for you (if you alone in this chat, chat will be delete\n";
}