#include "Client.h"

Client::Client(std::string api, std::string pid)
{
	this->api = api;
	this->proj_id = pid;
	Crypto::init();
}

void Client::loginUser(const std::string& username, const std::string& password, const std::string& current_mac)
{
	
	if (!isUsernameExists(username))
	{
		throw std::invalid_argument("User not found\n");
	}
	if (!Crypto::verifyPassword(password, getUserField(username,"password")["stringValue"]))
	{
		throw std::invalid_argument("User not found\n");
	}

	if (!isMacAllowed(username, current_mac))
	{
		addMAC(username, current_mac);
		throw std::invalid_argument("NO ACCESS\nP.S.\nRequest to get was sent\n");
	}
}

std::vector<std::string> Client::getHeaders(const std::string& username)
{
	std::vector<std::string> headers;
	json Ids = getUserField(username, "chatsId");
	for (auto& id : Ids["arrayValue"]["values"]) 
	{
		json header = getChatField(id["stringValue"], "name");
		headers.push_back(header["stringValue"]);
	}
	return headers;
}

void Client::registerUser(const std::string& username, const std::string& password, const std::vector<std::string>& macs) {
	if (isUsernameExists(username)) {
		throw std::invalid_argument("Username \"" + username + "\" already exists\n");
	}

	std::vector<unsigned char> private_key, public_key;
	Crypto::generateKeyPair(macs, private_key, public_key);

	json user_data = {
		{"password", Crypto::hashPassword(password)},
		{"public_key", Crypto::base64Encode(public_key)},
		{"allowed_macs", macs},
		{"requests", json::array()},
		{"chat_invites", json::array()},
		{"chatsId", json::array()}
	};

	if (!saveToFirestore("users", username, user_data)) {
		throw std::runtime_error("Failed to register user\n");
	}
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
					macAddressStream << ":";
				}
				macAddressStream << std::hex << static_cast<int>(pCurrAddresses->PhysicalAddress[i]);
			}
			return macAddressStream.str();
		}
	}
	throw std::runtime_error("Can`t getting MAC address");
}

bool Client::isMacAllowed(const std::string& username, const std::string& target_mac) {

	json allowed_macs_field = getUserField(username, "allowed_macs");
	std::vector<std::string> allowed_macs;
	if (!allowed_macs_field.empty() && allowed_macs_field.contains("arrayValue")) {
		for (const auto& item : allowed_macs_field["arrayValue"]["values"]) {
			allowed_macs.push_back(item["stringValue"].get<std::string>());
		}
	}
	return std::find(allowed_macs.begin(), allowed_macs.end(), target_mac) != allowed_macs.end();
}

bool Client::saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data)
{
	CURL* curl = curl_easy_init();
	if (!curl) return false;

	
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/" + collection +
		"?documentId=" + doc_id + "&key=" + api;

	
	json firestore_doc;
	firestore_doc["fields"] = json::object();

	for (const auto& [key, value] : data.items()) {
		if (key == "chat_invites") {
			json invites_array;
			for (const auto& invite : value) {
				json invite_map;
				invite_map["mapValue"] = {
					{"fields", {
						{"chat_id", {{"stringValue", invite["chat_id"].get<std::string>()}}},
						{"sender", {{"stringValue", invite["sender"].get<std::string>()}}
					}}
				}};
				invites_array.push_back(invite_map);
				}
				firestore_doc["fields"][key] = { {"arrayValue", {{"values", invites_array}}} };
			}
		else if (value.is_array()) {
			json array_values;
			for (const auto& item : value) {
				array_values.push_back({ {"stringValue", item} });
			}
			firestore_doc["fields"][key] = { {"arrayValue", {{"values", array_values}}} };
		}
		else if (value.is_string()) {
			firestore_doc["fields"][key] = { {"stringValue", value.get<std::string>()} };
		}
		}
	
		
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		
		std::string json_data = firestore_doc.dump();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		
		CURLcode res = curl_easy_perform(curl);
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		
		if (http_code != 200) {
			return false;
		}

		return true;
	}

json Client::getUserDocument(const std::string& username)
{
	curl = curl_easy_init();
	std::string response;
	std::string url = "https://firestore.googleapis.com/v1/projects/" +
		proj_id + "/databases/(default)/documents/users/" +
		username + "?key=" + api;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);

	if (http_code != 200) return json();

	json document = json::parse(response);

	return parseFirestoreFields(document["fields"]);
}

size_t Client::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
	size_t total_size = size * nmemb;
	output->append(static_cast<char*>(contents), total_size);
	return total_size;
}

json Client::parseFirestoreFields(const json& fields) 
{
	json result;
	for (auto& [key, value] : fields.items()) {
		if (value.contains("stringValue")) {
			result[key] = value["stringValue"];
		}
		else if (value.contains("arrayValue")) {
			std::vector<std::string> items;
			if(!value["arrayValue"].empty())
			for (auto& item : value["arrayValue"]["values"]) {
				items.push_back(item["stringValue"]);
			}
			result[key] = items;
		}
	}
	return result;
}

bool Client::isUsernameExists(const std::string& username)
{
	CURL* curl = curl_easy_init();
	std::string response;

	std::string url = "https://firestore.googleapis.com/v1/projects/"+this->proj_id+"/databases/(default)/documents/users/" + username +
		"?mask.fieldPaths=__name__&key="+api;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		throw std::runtime_error("Can`t acces to server\n");
	}

	return (http_code == 200);
}

json Client::getUserField(const std::string& username, const std::string& field)
{
	CURL* curl = curl_easy_init();
	std::string response;
	std::string url =
		"https://firestore.googleapis.com/v1/projects/"+proj_id+
		"/databases/(default)/documents/users/" + username +
		"?mask.fieldPaths=" + field +
		"&key="+api;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);

	if (http_code != 200) return json();

	json document = json::parse(response);
	return document["fields"][field];
}

json Client::getChatField(const std::string& chatId, const std::string& field)
{
	CURL* curl = curl_easy_init();
	std::string response;
	std::string url =
		"https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/chats/" + chatId +
		"?mask.fieldPaths=" + field +
		"&key=" + api;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);

	if (http_code != 200) return json();

	json document = json::parse(response);
	return document["fields"][field];
}

void Client::addMAC(const std::string& username, const std::string& mac) {
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=requests&key=" + api;
	curl_free(escaped_username);

	
	
	json requests_field = getUserField(username, "requests");
	std::vector<std::string> requests;
	if (!requests_field.empty() && requests_field.contains("arrayValue")) {
		for (const auto& item : requests_field["arrayValue"]["values"]) {
			requests.push_back(item["stringValue"].get<std::string>());
		}
	}
	
	if (std::find(requests.begin(), requests.end(), mac) != requests.end()) {
		return;
	}
	requests.push_back(mac);


	json body = {
		{"fields", {
			{"requests", {
				{"arrayValue", {
					{"values", json::array()}
				}}
			}}
		}}
	};


	for (const auto& req : requests) {
		body["fields"]["requests"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
	}


	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");


	std::string request_body = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"); // Используем PATCH вместо PUT
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


	std::string response;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);


	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);


	if (http_code != 200) {
		throw std::runtime_error(
			"Server Error (" + std::to_string(http_code) + ")\n");
	}
}
void Client::deleteUser(const std::string& username)
{
	json Ids = getUserField(username, "chatsId");
	for (auto& id : Ids["arrayValue"]["values"])
		decreaseChatUsers(id["stringValue"]);
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	
	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/"+proj_id+
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?key="+api;
	curl_free(escaped_username);

	
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	
	std::string response;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	
	if (http_code != 200) {
		throw std::runtime_error(
			"Failed to delete user. HTTP code: " + std::to_string(http_code) +
			"\nResponse: " + response
		);
	}
}

void Client::decreaseChatUsers(const std::string& id)
{
	json amount = getChatField(id, "usersAmount");
	std::string s = amount["stringValue"];
	long long kol = std::stoll(s);
	kol--;
	if (kol <= 0)
	{
		deleteChat(id);
	}
	else
	{
		updateChatUserAmount(id, std::to_string(kol));
	}
}

void Client::updateChatUserAmount(const std::string& id, const std::string& kol)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_id = curl_easy_escape(curl, id.c_str(), id.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/chats/" + escaped_id +
		"?updateMask.fieldPaths=usersAmount&key=" + api;
	curl_free(escaped_id);

	json body = {
		{"fields", {
			{"usersAmount", {
				{"stringValue", kol}
			}}
		}}
	};

}

void Client::deleteChat(const std::string& id)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_id = curl_easy_escape(curl, id.c_str(), id.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/"+proj_id+
		"/databases/(default)/documents/chats/" + std::string(escaped_id) +
		"?key="+api;
	curl_free(escaped_id);

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	
	std::string response;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	
	if (http_code != 200) {
		throw std::runtime_error(
			"Failed to delete chat. HTTP code: " + std::to_string(http_code) +
			"\nResponse: " + response
		);
	}
}

void Client::createChat(const std::string& name, const std::string& username)
{
	const std::string chatId = findChatsId();

	
	json user_data = {
		{"name", name},
		{"usersAmount","1"},
		{"messages", json::array()},

	};

	if (!saveToFirestore("chats", chatId, user_data))
		throw std::runtime_error("Can`t acces to server\n");
	addChat(chatId, username);
}

std::string Client::findChatsId()
{
	long long id = 0;
	while (id != LLONG_MAX)
	{
		if (isChatExists(std::to_string(id)))
			id++;
		else
			return std::to_string(id);
	}
}

bool Client::isChatExists(const std::string& id)
{
	CURL* curl = curl_easy_init();
	std::string response;

	std::string url = "https://firestore.googleapis.com/v1/projects/" + this->proj_id + "/databases/(default)/documents/chats/" + id +
		"?mask.fieldPaths=__name__&key=" + api;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		throw std::runtime_error("Can`t acces to server\n");
	}

	return (http_code == 200);
}

void Client::addChat(const std::string& id, const std::string& username)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=chatsId&key=" + api;
	curl_free(escaped_username);



	json requests_field = getUserField(username, "chatsId");
	std::vector<std::string> requests;
	if (!requests_field.empty() && requests_field.contains("arrayValue")) {
		for (const auto& item : requests_field["arrayValue"]["values"]) {
			requests.push_back(item["stringValue"].get<std::string>());
		}
	}

	if (std::find(requests.begin(), requests.end(), id) != requests.end()) {
		return;
	}
	requests.push_back(id);


	json body = {
		{"fields", {
			{"chatsId", {
				{"arrayValue", {
					{"values", json::array()}
				}}
			}}
		}}
	};


	for (const auto& req : requests) {
		body["fields"]["chatsId"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
	}


	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");


	std::string request_body = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"); // Используем PATCH вместо PUT
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


	std::string response;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);


	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);


	if (http_code != 200) {
		throw std::runtime_error(
			"Server Error (" + std::to_string(http_code) + ")\n");
	}
}