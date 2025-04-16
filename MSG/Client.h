#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "Image.h"
#include "Text.h"
#include "Crypto.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

class Client
{
public:
	static void init();
	static std::string getMAC();
	//Requests
	static bool userExist(std::string,std::string);
	static int checkDevice();
	static std::vector<std::string> getUserData(std::string);
	static void sendVerifyRequest(std::string);
	static void addUser(std::string, std::string, std::string, std::string);
	static void verifyDevice(std::string, std::string, bool);
	static std::string getChatHeader(std::string);
	static std::vector<std::unique_ptr<Message>> getMessages(std::string);
	static void deleteChat(std::string id);
	static void sendMessage(std::string id,std::string sender, std::string type, std::string data, std::string format ="");
private:
	static int port;
	static std::string ip;
	static SOCKET sock;
	static sockaddr_in serv_addr;

	static void sendMessage(const std::string& message);
	static std::string receiveMessage();
	static void connectToServer();
	static void closeConnection();

};

