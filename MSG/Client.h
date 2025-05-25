#pragma once

#include <WinSock2.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "Image.h"
#include "Text.h"
#include "Crypto.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using json = nlohmann::json;

class Client
{
public:
	Client(std::string,std::string);
	std::string getMAC();
	//Requests
	void registerUser(const std::string& username, const std::string& password, const std::vector<std::string>& macs);
	void loginUser(const std::string& username, const std::string& password, const std::string& current_mac);
	std::vector<std::string> getHeaders(const std::string& username);
	bool isUsernameExists(const std::string& username);
	bool isUserOnline(const std::string& username);
	json getUserField(const std::string& username, const std::string& field);
	json getChatField(const std::string& chatId, const std::string& field);
	void addMAC(const std::string& username, const std::string& mac);
	void setRequests(const std::string& username, std::vector<std::string>q);
	void addAllowedMAC(const std::string& username, const std::string& mac);
	void deleteUser(const std::string& username);
	void deleteChat(const std::string& id);
	void updateChatUserAmount(const std::string& id, const std::string& kol);
	void createChat(const std::string& chatName, const std::string& username);
	bool isChatExists(const std::string& id);
	void addChat(const std::string& id, const std::string& username);
	std::vector<std::string> getRequests(const std::string& username);
	void updateTime(const std::string& username);
	time_t nowTime() const;
private:
	std::string findChatsId();
	void decreaseChatUsers(const std::string& id);
	json getUserDocument(const std::string& username);
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
	json parseFirestoreFields(const json& fields);
	bool isMacAllowed(const std::string& username, const std::string& target_mac);
	bool saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data);

	std::string api;
	std::string proj_id;
	CURL* curl = nullptr;
};

