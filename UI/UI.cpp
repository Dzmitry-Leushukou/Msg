#include "UI.h"

void UI::start()
{
	while (true)
	{
		if (!app->authorized())
			authorize();

	}
}

void UI::authorize()
{
	LoginForm* lf = new LoginForm(*app);
	lf->start();
	//user = lf->getUser();
	delete lf;
	lf = nullptr;
	std::cout << "Authorized!!!\n";
	
}

UI::~UI()
{
	delete app;
	app = nullptr;
}