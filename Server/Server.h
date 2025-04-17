#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

class Server 
{
public:
    Server(int port);
    void start();
    void stop();

private:
    std::vector<std::string> process(std::string);

    int port;
    SOCKET serverSocket;
    sockaddr_in serverAddr;

    void handleClient(SOCKET clientSocket);
};
