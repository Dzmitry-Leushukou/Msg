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

    if (message.find(" ") >= message.size())
    {
        response = getUserData(message);
        return response;
    }

    if (message.size() > 2)
    {
        std::string type;
        type = message[message.size() - 2] + message[message.size() - 1];
        if (type == " ?")
        {
            
            if (message.find(" ") < message.size()&& message.find(" ")!= message.find(" ?"))
            {
                response.push_back(std::to_string(userExist(message)));
                return response;
            }

            response.push_back(std::to_string(checkDevice(message)));
            return response;
            
        }
    }

    if (message.size() > 3)
    {
        std::string type;
        type = message[message.size() - 3] + message[message.size() - 2] + message[message.size() - 1];
        if (type == " h?")
        {

            if (message.find(" ") < message.size() && message.find(" ") != message.find(" ?"))
            {
                response = getHeader(message);
                return response;
            }
        }
    }

    std::reverse(response.begin(), response.end());
    return response;
}

std::vector<std::string>Server::getHeader(std::string id)//Header by chatID
{
    //Request to db
    std::vector<std::string>response;
    return response;
}

std::vector<std::string>Server::getUserData(std::string login)
{
    //request to db
    std::vector<std::string>ans;
    ans.push_back("...end...");
    return ans;
}

int Server::checkDevice(std::string message)
{
    std::string MAC;
    for (auto& i : MAC)
        if (i == ' ')
            break;
        else
            MAC += i;
    
    //Request to db
    int verdict=0;
    return verdict;
}

bool Server::userExist(std::string message)
{
    std::string username;
    std::string password;
    int i = 0;
    while (message[i] != ' ')
    {
        username += message[i];
        i++;
    }
    i++;
    while (message[i] != ' ')
    {
        password += message[i];
        i++;
    }
    
    //Request to db
    bool verdict = false;
    return verdict;
}