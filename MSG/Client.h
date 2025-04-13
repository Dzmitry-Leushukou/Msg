#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

class Client
{
public:
	static void init();
	static std::string getMAC();
	//Requests
	static bool userExist(std::string);
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

