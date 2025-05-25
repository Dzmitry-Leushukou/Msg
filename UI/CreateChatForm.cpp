#include "CreateChatForm.h"

CreateChatForm::CreateChatForm(Application& app)
{
	this->app = &app;
}

void CreateChatForm::start()
{
	clearScreen();
	std::cout << "=Chat creation=\n";
	name = getString("Write name of the chat(nothing to cancel):", 0);
}

std::string CreateChatForm::getName()const
{
	return name;
}
