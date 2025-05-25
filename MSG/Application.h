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
	void setUser(std::string login);
	void regUser(std::string login, std::string password);
	void loginUser(std::string login, std::string password);
	bool authorized() const;
	std::vector<std::string> getChatHeaders();
	void deleteUser();
private:

	User* user = nullptr;
	std::string curMAC;
	Client* client = nullptr;
	std::string skey_path;
};

