#include "RequestsMenu.h"
RequestsMenu::RequestsMenu(Application& app)
{
	this->app = &app;
}

void RequestsMenu::start()
{
	clearScreen();
	while (true) 
	{
		std::vector<std::string>q = app->getRequests();
		std::string s;
		clearScreen();
		std::cout << "[Request]\n";
		if (q.empty())
			std::cout << "No requests\n";
		else
			std::cout << q.at(0)<<'\n';
		std::cout << "-------------------------------\n";
		help();
		std::getline(std::cin, s);
		if (s == "q")
			return;
		if (s == "y")
		{
			if(!q.empty())
				app->addAllowedMAC(q.at(0));
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
		if (!q.empty())
		q.erase(q.begin());
		app->updateRequests(q);
	}
}

void RequestsMenu::help()
{
	std::cout << "Command | Description\n";
	std::cout << "q | quit\n";
	std::cout << "y | accept\n";
	std::cout << "n | decline\n";
}