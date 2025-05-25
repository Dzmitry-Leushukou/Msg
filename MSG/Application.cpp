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

void Application::regUser(const std::string login, const std::string password)
{
	client->registerUser(login, password, {curMAC});
	loginUser(login, password);
}

void Application::loginUser(const std::string login, const std::string password)
{
	std::pair<std::vector<unsigned char>, json>login_data = client->loginUser(login, password, curMAC);
	FileService::saveToFile(skey_path, Utils::to_string(login_data.first));
	user = new User(login_data.second);
}

bool Application::authorized() const
{
	return user != nullptr;
}

std::vector<std::string> Application::getChatHeaders()
{
	return {};
}