#include "Application.h"

Application::~Application()
{
	delete client;
	delete user;
	user = nullptr;
	client = nullptr;
	userOnline.detach();
}

Application::Application():userOnline(&Application::updUserStatus, this)
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

std::vector<std::string>Application::getRequests()
{
	return client->getRequests(user->getUsername());
}

void Application::addAllowedMAC(const std::string& MAC)
{
	client->addAllowedMAC(user->getUsername(),MAC);
}

void Application::updateRequests(std::vector<std::string>q)
{
	client->setRequests(user->getUsername(),q);
}

void Application::clearUser()
{
	delete user;
	user = nullptr;
}

void Application::updUserStatus()
{
	while (true)
	{
		if (user)
		{
			client->updateTime(user->getUsername());
		}
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

std::pair<std::string, std::string> Application::getInvite()
{
	return client->getInvite(user->getUsername());
}

std::string Application::getChatName(const std::string& id)const
{
	return client->getChatName(id);
}

void Application::popInvite()
{
	client->popInvite(user->getUsername());
}

void Application::acceptInvite(const std::string& id)
{
	client->addChat(id, user->getUsername());
}