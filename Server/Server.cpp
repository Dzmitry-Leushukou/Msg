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
        
        if (type == " -")
        {
            deleteChat(message);
            return {};
        }

        if (type == " +")
        {
            addVerifyRequest(message);
            return {};
        }
    }

    if (message.size() > 3)
    {
        std::string type;
        type = message[message.size() - 3] + message[message.size() - 2] + message[message.size() - 1];
        
        if (type == " h?")
        {
            response = getHeader(message);
            return response;
        }

        if (type == " m?")
        {
            getMessages(message);
            response.push_back("...end...");
            return response;
        }

        if (type == " m+")
        {
            sendMessage(message);
            return {};
        }

        if (type == " u+")
        {
            createUser(message);
            return {};
        }

        if (type == " v+")
        {
            verifyDevice(message);
            return {};
        }

        if (type == " i+")
        {
            sendInvite(message);
            return {};
        }

        if (type == " i?")
        {
            return getInvite(message);
        }

        if (type == " c+")
        {
            addChat(message);
            return {};
        }
    }
    if (message.size() > 4) 
    {
        std::string type;
        type += message[message.size() - 4] + message[message.size() - 3] + message[message.size() - 2] + message[message.size() - 1];
        if (type == " ac+")
        {
            addChat(message);
            return {};
        }
    }

}

void Server::createChat(std::string message)
{
    std::string id;
    std::string chatID;
    int i = 0;
    while (message[i] != ' ')
    {
        id += message[i];
        i++;
    }
    i++;
    //create chat & userid to chat & chatid to user

}

void Server::addChat(std::string message)
{
    std::string id;
    std::string chatID;
    int i = 0;
    while (message[i] != ' ')
    {
        id += message[i];
        i++;
    }
    i++;
    while (message[i] != ' ')
    {
        id += message[i];
        i++;
    }

    //add chatID to user chats && add userid to chat
}

std::vector<std::string> Server::getInvite(std::string message)
{
    std::string sender;
    std::string chatID;
    int i = 0;
    while (message[i] != ' ')
    {
        sender += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        chatID += message[i];
    }
    i++;
    if (chatID == "")
        chatID = "...end...";
    return { sender,chatID };

}

void Server::sendInvite(std::string message)
{
    std::string sender;
    std::string receiver;
    std::string chatID;

    int i = 0;
    while (message[i] != ' ')
    {
        sender += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        receiver += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        chatID += message[i];
    }
    i++;
    
    //add invite to invite list in receiver by sender to chatid
}

void Server::verifyDevice(std::string message)
{
    std::string login;
    std::string addres;
    std::string verdict;

    int i = 0;
    while (message[i] != ' ')
    {
        login += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        addres += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        verdict += message[i];
    }
    i++;

    //add addres to login ith list (-1/0/1)
}

void Server::createUser(std::string message)
{
    std::string login;
    std::string password;
    std::string publicKey;
    std::string privateKey;

    int i = 0;
    while (message[i] != ' ')
    {
        login += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        password += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        publicKey += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        privateKey += message[i];
    }
    i++;

    //create user in db
}

void Server::addVerifyRequest(std::string message)
{
    std::string login;
    std::string mac;
    int i = 0;
    while (message[i] != ' ')
    {
        login += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        mac += message[i];
    }
    //request to db: add to user with login mac to list of verifies
}

void Server::sendMessage(std::string message)
{
    std::string id;
    std::string sender;
    std::string type;
    std::string data;
    std::string format;
    int i = 0;
    while (message[i] != ' ')
    {
        id += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        sender += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        type += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        data += message[i];
    }
    i++;
    while (message[i] != ' ')
    {
        format += message[i];
    }
    i++;

    //requets to add type message sent by sender in data on chat with id

}

void Server::deleteChat(std::string id)
{
    //request to db to delete chat for all users & delete chat field

}

std::vector<std::string>Server::getMessages(std::string req)
{
    std::string chatID,time;
    int i = 0;
    while (req[i] != ' ')
    {
        chatID += req[i];
        i++;
    }
    i++;
    while (req[i] != ' ')
    {
        time += req[i];
        i++;
    }

    std::vector<std::string>ans;
    //request to db
    reverse(ans.begin(), ans.end());
    return ans;
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