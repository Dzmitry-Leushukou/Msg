#include "LoginForm.h"

void LoginForm::start()
{
	while (!user)
	{
		main();
		std::string login = username();
		std::string pass =  password();
		std::string method = getMethod();
		if (method=="login" && correct(login,pass)&&verified(login))
		{
			user = new User(login, pass);
			return;
		}
		if (method == "register" && !exist(login))
		{
			user = new User(login, pass);
			return;
		}
		std::cout << "Something went wrong try again...\n";
		pause();
	}
}

User* LoginForm::getUser() const
{
	return user;
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

bool LoginForm::verified(std::string login)
{
	return false;
}
bool LoginForm::exist(std::string login)
{
	return true;
}
bool LoginForm::correct(std::string login, std::string pass)
{
	return false;
}