#include "Client.h"

int Client::port = 0;
std::string Client::ip = "";
SOCKET Client::sock = INVALID_SOCKET;
sockaddr_in Client::serv_addr;

void Client::init()
{
	try
	{
		std::ifstream fin("config");
		std::string s;
	
		if (fin >> s)
		{
			ip = s;
		}
		else 
		{
			fin.close();
			throw std::invalid_argument("Invalid config file"); 
		}

		if (fin >> s)
		{
			fin.close();
			port = stoi(s);
		}
		else
		{
			fin.close();
			throw std::invalid_argument("Invalid config file");
		}
		
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file");
	}

	connectToServer();
}

void Client::connectToServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		throw std::runtime_error("WSAStartup failed");
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		throw std::runtime_error("Failed to create socket");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
		closesocket(sock);
		WSACleanup();
		throw std::invalid_argument("Invalid IP address");
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		closesocket(sock);
		WSACleanup();
		throw std::runtime_error("Failed to connect to the server");
	}
}

void Client::sendMessage(const std::string& message) 
{
	if (send(sock, message.c_str(), message.length(), 0) == SOCKET_ERROR)
	{
		throw std::runtime_error("Failed to connect the server: " + std::to_string(WSAGetLastError()));
	}
}

std::string Client::receiveMessage()
{
	char buffer[1024] = { 0 };
	int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);

	if (bytesReceived == SOCKET_ERROR) 
	{
		throw std::runtime_error("Failed to connect the server: " + std::to_string(WSAGetLastError()));
	}

	return std::string(buffer, bytesReceived);
}

void Client::closeConnection() 
{
	if (sock != INVALID_SOCKET) 
	{
		if (closesocket(sock) == SOCKET_ERROR) 
		{
			throw std::runtime_error("Failed to close socket: " + std::to_string(WSAGetLastError()));
		}
		sock = INVALID_SOCKET; 
	}
	WSACleanup(); 
}

bool Client::userExist(std::string username, std::string password)
{
	sendMessage(username + " " + password + " ?");
	std::string answer = receiveMessage();
	return stoi(answer);
}

int Client::checkDevice()
{
	sendMessage(getMAC() + " ?");
	std::string answer = receiveMessage();
	return stoi(answer);
}

std::vector<std::string> Client::getUserData(std::string login)
{
	sendMessage(login);
	std::vector<std::string>data;
	while (true)
	{
		std::string s = receiveMessage();
		if (s != "...end...")
			data.push_back(s);
		else
			return data;
	}
}

std::string Client::getChatHeader(std::string id)
{
	sendMessage(id + " h?");
	return receiveMessage();
}

std::vector<std::unique_ptr<Message>> Client::getMessages(std::string chatID,std::string time)
{
	sendMessage(chatID + " " + time + " m?");
	std::vector<std::unique_ptr<Message>>msg;
	while (true)
	{
		std::string r = receiveMessage();
		if (r == "...end...")
			return msg;
		std::string type = receiveMessage();
		std::string data = receiveMessage();
		std::string time = receiveMessage();
		if (type == "image") 
		{
			std::string format = receiveMessage();
			msg.push_back(std::make_unique<Image>(r,type,Crypto::decrypt(data),time));
				continue;
		}
		msg.push_back(std::make_unique<Text>(r, Crypto::decrypt(data),time));
	}

}

void Client::deleteChat(std::string id)
{
	sendMessage(id + " -");
}

void Client::sendMessage(std::string id, std::string sender, std::string type, std::string data, std::string format)
{
	sendMessage(id + " " + sender + " " + type + " " + data + " " + format + " m+");
}

void Client::sendVerifyRequest(std::string login)
{
	sendMessage(login + " " + getMAC() + " +");
}

void Client::addUser(std::string login, std::string password,
					 std::string publicKey, std::string privateKey)
{
	sendMessage(login + " " + password + " " + publicKey + " " + privateKey + " u+");
}

void Client::verifyDevice(std::string login, std::string address, bool verdict)
{
	sendMessage(login + " " + address + " " + std::to_string(verdict) + " v+");
}

std::string Client::getMAC()
{
	ULONG bufferSize = 0;
	GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &bufferSize);
	std::vector<BYTE> buffer(bufferSize);

	IP_ADAPTER_ADDRESSES* pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &bufferSize) == NO_ERROR)
	{
		std::ostringstream macAddressStream;

		for (IP_ADAPTER_ADDRESSES* pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			for (int i = 0; i < pCurrAddresses->PhysicalAddressLength; i++)
			{
				if (i > 0) 
				{
					macAddressStream << "-";
				}
				macAddressStream << std::hex << static_cast<int>(pCurrAddresses->PhysicalAddress[i]);
			}
			return macAddressStream.str();
		}
	}
	throw std::runtime_error("Can`t getting MAC address");
}

void Client::sentInvite(std::string sender, std::string receiverId,std::string chatID)
{
	sendMessage(sender + " " + receiverId + chatID +" i+");
}

std::pair<std::string,std::string> Client::getInvite(std::string id)
{
	sendMessage(id + " i?");
	std::string sender = receiveMessage();
	if (sender == "...end...")
		return {"",""};
	std::string idOfChat = receiveMessage();
	if (idOfChat == "...end...")
		return { sender,"" };
	return { sender,idOfChat};
}
void Client::createChat(std::string sender, std::string id,std::string chat)
{
	if (chat == "")
	{
		sendMessage(sender + " c+");
	}
	sendMessage(id +" " + chat + " ac+");

}
std::string Client::getDevice(std::string id)
{
	sendMessage(id + " d?");
	std::string ans = receiveMessage();
	if (ans == "...end...")
	{
		return "";
	}
	return ans;
}

void Client::deleteUser(std::string id)
{
	sendMessage(id + " u-");
}