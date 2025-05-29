#include "Application.h"

Application::~Application()
{
	delete client;
	delete user;
	user = nullptr;
	client = nullptr;
}

Application::Application():userOnline(&Application::updUserStatus, this)
{
	std::vector<std::string>tmpData = FileService::getConfigInfo(4);
	this->skey_path = tmpData.at(2);
	this->key_path = tmpData.at(3);
	client = new Client(tmpData.at(0), tmpData.at(1), tmpData.at(3),tmpData.at(2));
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

std::string Application::getRequests()
{
	return client->getRequests(user->getUsername());
}

void Application::addAllowedMAC()
{
	client->addAllowedMAC(user->getUsername());
	client->updateKeys(user->getUsername());
}

void Application::popRequest()
{
	client->popRequest(user->getUsername());
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
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}

std::vector<std::string> Application::getInvite()
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

void Application::acceptInvite(const std::string& id, const std::string& key)
{
	client->addChat(id,Crypto::base64Decode(key), user->getUsername()); 
	
}

std::vector<std::unique_ptr<Message>>Application::getNewMessage(time_t lastUpdateTime)
{
	return client->getNewMessages(choosed_chat, lastUpdateTime);
}

void Application::sendMessage(const std::string& id, const std::string& message)
{
	//return client->getNewMessages(choosed_chat, lastUpdateTime);
}
void Application::loadChat(unsigned int id)
{
	choosed_chat = client->loadChat(user->getUsername(),id);
}