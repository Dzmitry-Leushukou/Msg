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