#include "Application.h"


Application::~Application()
{
	delete client;
	delete user;
	user = nullptr;
	client = nullptr;

}

Application::Application()
{
	client = new Client();
	this->curMAC = client->getMAC();
	std::cout << curMAC<<'\n';
	system("pause");
}

void Application::setUser(std::string login)
{
	this->user = new User(login);
}

void Application::regUser(std::string login, std::string password)
{
	//create account (create table && verified current pc)
	client->registerUser(login, password, {curMAC});
	loginUser(login, password);
	//regen private key
}

void Application::loginUser(std::string login, std::string password)
{

	user = new User(login);
}
bool Application::authorized() const
{
	return user != nullptr;
}