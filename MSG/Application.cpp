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
	std::vector<std::string>tmpData = FileService::getConfigInfo(3);
	this->skey_path = tmpData.at(2);
	client = new Client(tmpData.at(0), tmpData.at(1));
	this->curMAC = client->getMAC();
}

void Application::setUser(std::string login)
{
	this->user = new User(login);
}

void Application::regUser(const std::string login, const std::string password)
{
	FileService::saveToFile(skey_path,Utils::to_string(client->registerUser(login, password, {curMAC})));
	//loginUser(login, password);
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