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
	std::vector<unsigned char> registerUser(const std::string& username, const std::string& password, const std::vector<std::string>& macs);
    bool saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data); 

private:
	bool checkMacAddress(const json& allowed_macs, const std::string& mac);
	std::string api;
	std::string proj_id;
	CURL* curl = nullptr;
};

