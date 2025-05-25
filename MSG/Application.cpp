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

void Application::regUser(const std::string& login, const std::string& password)
{
	client->registerUser(login, password, {curMAC});
	loginUser(login, password);
}

void Application::loginUser(const std::string& login, const std::string& password)
{
	client->loginUser(login, password, curMAC);
	user = new User(login);
}

bool Application::authorized() const
{
	return user != nullptr;
}

std::vector<std::string> Application::getChatHeaders()
{
	return client->getHeaders(user->getUsername());
}

void Application::deleteUser()
{
	client->deleteUser(user->getUsername());
	delete user;
	user = nullptr;
}

void Application::createChat(const std::string& chatName)
{
	client->createChat(chatName,user->getUsername());
}