#include "UI.h"
void UI::start()
{
	while (true)
	{
		if (!user)
			authorize();

	}
}

void UI::authorize()
{
	LoginForm* lf = new LoginForm();
	lf->start();
	user = lf->getUser();
	delete lf;
	lf = nullptr;
}