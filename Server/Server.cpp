#include "Server.h"

Server::Server(int port) : port(port), serverSocket(INVALID_SOCKET)
{
    
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Failed to create socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        throw std::runtime_error("Failed to listen on socket");
    }
}

void Server::start()
{
    std::cout << "Server started on port " << port << std::endl;

    while (true)
    {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET)
        {
            std::cout << "Client connected." << std::endl;
            handleClient(clientSocket);
        }
        else
        {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        }
    }
}

void Server::stop()
{
    closesocket(serverSocket);
    WSACleanup();
    std::cout << "Server stopped." << std::endl;
}

void Server::handleClient(SOCKET clientSocket)
{
    char buffer[1024];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesRead] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        std::vector<std::string> response = process(buffer);
        while (!response.empty())
        {
            send(clientSocket, response.back().c_str(), response.size(), 0);
            response.pop_back();
        }
 
    }

    std::cout << "Client disconnected." << std::endl;
    closesocket(clientSocket);
}

std::vector<std::string> Server::process(std::string message)
{
    std::vector<std::string>response;



    std::reverse(response.begin(), response.end());
    return response;
}