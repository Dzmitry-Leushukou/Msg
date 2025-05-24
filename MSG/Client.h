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
	std::pair<std::vector<unsigned char>, json> loginUser(const std::string& username, const std::string& password, const std::string& current_mac);
    bool saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data); 

private:
	json getUserDocument(const std::string& username);
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
	json parseFirestoreFields(const json& fields);
	bool checkMacAddress(const json& allowed_macs, const std::string& mac);

	std::string api;
	std::string proj_id;
	CURL* curl = nullptr;
};

