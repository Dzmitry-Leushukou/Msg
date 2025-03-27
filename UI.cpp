#include "UI.h"

UI::UI()
{
	//include multilanguge interface
	std::setlocale(LC_ALL, ""); 

}

void UI::inputHandler()
{
	while (true)
	{
		show();
	}
	
}

void UI::show()
{
	system("cls");

	if (!user)
	{
		login();
		return;
	}
}

void UI::login()
{
	system("cls");
	std::cout << "==Login==\n";
	auto[username, password] = userForm();

	try
	{
		DBService::checkUser(username, password);
	}
	catch (const std::invalid_argument& e)
	{
		std::cout << "Wrong username/password. Try again.\nFor register type Y/y and press enter, otherwise put any key(s) and press enter\n";
		std::string key;
		std::cin >> key;
		if (key == "y" || key == "Y")
		{
			registration();
		}
		else
			return;
	}
	user = new User(DBService::getUser(username, password));
}

void UI::registration()
{
	system("cls");
	std::cout << "==Registration==\nUsername - unique sequence of characters (1 or more)\n"
		"Password - sequence of at least 4 characters\n";
	auto [username, password] = userForm();


		std::cout << "To go back to login page write Y/y and press Enter\nFor register put any key(s) and Enter\n";
		std::string key;
		std::cin >> key;
		if (key == "y" || key == "Y")
		{
			return; // Back to handler => back to login form
		}
		else
		{
			if (username.size() >= 1 && password.size() >= 4)
			{
				try
				{
					DBService::checkUser(username, password);
				}
				catch (const std::invalid_argument& e) //User not found =>
				{
					//create account
					user = new User(DBService::getUser(username, password));
					DBService::createUser(user);
					return;
				}
				std::cout << "Username is already in use\n";
				system("pause");
			}
			else
			{
				std::cout << "Username must has 1 or more characters. Password must has 4 or more characters\n";
				system("pause");
			}
			registration();
		}
}

std::pair<std::string,std::string> UI::userForm()
{
	std::string username;
	std::string password;
	std::cout << "Username: ";
	std::cin >> username;
	std::cout << "Password: ";
	std::cin >> password;
	return { username,password };
}