#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
	static void init();
	
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

