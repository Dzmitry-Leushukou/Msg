#pragma once

#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif

#include "User.h"
#include "Client.h"
#include "FileService.h"
#include "Utils.h"

class EXP Application
{
public:
	Application();		
	~Application();
	void setUser(const std::string& login);
	void regUser(const std::string& login, const std::string& password);
	void loginUser(const std::string& login, const std::string& password);
	bool authorized() const;
	std::vector<std::string> getChatHeaders();
	void deleteUser();
	void createChat(const std::string& chatName);
	std::vector<std::string>getRequests();
	void addAllowedMAC(const std::string& MAC);
	void updateRequests(std::vector<std::string>q);
private:

	User* user = nullptr;
	std::string curMAC;
	Client* client = nullptr;
	std::string skey_path;
};

