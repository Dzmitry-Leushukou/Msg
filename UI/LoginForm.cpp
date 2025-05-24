#include "LoginForm.h"

LoginForm::LoginForm(Application & app)
{
	this->app = &app;
}

void LoginForm::start()
{
	while (true)
	{
		main();
		std::string login = username();
		std::string pass =  password();
		std::string method = getMethod();
		signView({login,pass,method});
		try {
			if (method == "login")
			{
				app->loginUser(login, pass);
				return;
			}
			if (method == "register")
			{
				app->regUser(login, pass);
				return;
			}
		}
		catch (const std::exception& e)
		{
			clearScreen();
			std::cout << e.what();
		}
		std::cout << "Something went wrong try again...\n";
		pause();
	}
}

void LoginForm::signView(std::vector<std::string>v)
{
	clearScreen();
	std::cout << "Try " << v.at(2) << "...\nLogin: " << v.at(0) << "\nPassword: " << v.at(1) << '\n';
}

void LoginForm::main()
{
	clearScreen();
	std::cout << "To sign in or registration write next data:\n";
}

std::string LoginForm::username()
{
	return getString("Write username: ");
}

std::string LoginForm::password()
{
	return getString("Write password: ",4);
}

std::string LoginForm::getMethod()
{
	if (getInt("Choose login method:\n1. Sign in\n2. Register\nMethod: ", 1, 2) == 1)
		return "login";
	return "register";	
}
