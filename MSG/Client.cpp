#include "Client.h"

Client::Client(std::string api, std::string pid, std::string key_path,std::string skey_path)
{
	this->api = api;
	this->proj_id = pid;
	this->key_path = key_path;
	this->skey_path = skey_path;
	Crypto::init();
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
}

Client::~Client()
{
	curl_easy_cleanup(curl);
}

void Client::loginUser(const std::string& username, const std::string& password, const std::string& current_mac)
{
	if (!isUsernameExists(username))
	{
		throw std::invalid_argument("User not found\n");
	}
	if(isUserOnline(username))
		throw std::invalid_argument("User already online\n");

	if (!Crypto::verifyPassword(password, getUserField(username,"password")["stringValue"]))
	{
		throw std::invalid_argument("User not found\n");
	}
	salt = Crypto::base64Decode(getUserField(username, "salt")["stringValue"]);
	std::vector<unsigned char> key = Crypto::deriveKeyFromPassword(password,salt);
	FileService::saveToFile(key_path, key);
	updateKeys(username);
	if (!isMacAllowed(username, current_mac))
	{
		addMAC(username, Crypto::encryptMAC(current_mac,key));
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

	std::vector<unsigned char> salt = Crypto::generateSalt();
	std::vector<unsigned char> key = Crypto::deriveKeyFromPassword(password,salt);
	auto encrypted_macs = Crypto::encryptAllMACs(macs, key);
	auto seed = Crypto::generateKeySeed(encrypted_macs, key);
	auto [public_key, private_key] = Crypto::generateEncryptionKeyPair(macs);

	json user_data = {
		{"password", Crypto::hashPassword(password)},
		{"salt", Crypto::base64Encode(salt)},
		{"public_key", Crypto::base64Encode(public_key)},
		{"allowed_macs", encrypted_macs},
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

bool Client::isMacAllowed(const std::string& username, const std::string& target_mac)
{
	json allowed_macs_field = getUserField(username, "allowed_macs");
	std::vector<std::string> allowed_macs;
	if (!allowed_macs_field.empty() && allowed_macs_field.contains("arrayValue")) {
		for (const auto& item : allowed_macs_field["arrayValue"]["values"]) {
			allowed_macs.push_back(Crypto::decryptMAC(item["stringValue"].get<std::string>(),FileService::loadFromFile(key_path)));
		}
	}
	return std::find(allowed_macs.begin(), allowed_macs.end(), target_mac) != allowed_macs.end();
}

bool Client::saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data)
{
	curl = curl_easy_init();
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
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		std::string response;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		curl_easy_perform(curl);
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
	curl = curl_easy_init();
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
	curl = curl_easy_init();
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
	curl = curl_easy_init();
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
	std::string emac = Crypto::encryptMAC(mac, FileService::loadFromFile(key_path));
	requests.push_back(emac);


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
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	std::string json_data = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
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
			"Failed to update usersAmount. HTTP code: " +
			std::to_string(http_code) + "\nResponse: " + response
		);
	}
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
	

	addChat(chatId, Crypto::encryptAsymmetric(getPublicKey(username), Crypto::generateChatKey()), username);
}

std::vector<unsigned char>Client::getPublicKey(const std::string& username)
{
	std::string k = getUserField(username, "public_key")["stringValue"].get<std::string>();
	return Crypto::base64Decode(k);
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

void Client::addChat(const std::string& id, std::vector<unsigned char>key, const std::string& username)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}
	// get Chats id

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

	//Get chatsKeys

	requests_field = getUserField(username, "chatsKey");
	std::vector<std::string> keys;
	if (!requests_field.empty() && requests_field.contains("arrayValue")) {
		for (const auto& item : requests_field["arrayValue"]["values"]) {
			keys.push_back(item["stringValue"].get<std::string>());
		}
	}
	keys.push_back(Crypto::base64Encode(key));


	json body = {
	{"fields", {
		{"chatsId", {
			{"arrayValue", {
				{"values", json::array()}
			}}
		}},
		{"chatsKey", {
			{"arrayValue", {
				{"values", json::array()}
			}}
		}}
	}}
	};

	//Fill body
	for (const auto& req : requests) {
		body["fields"]["chatsId"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
	}
	for (const auto& req : keys) {
		body["fields"]["chatsKey"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
	}

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Accept: application/json");

	std::string request_body = body.dump();
	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=chatsId" + 
		"&updateMask.fieldPaths=chatsKey" +
		"&key=" + api;
	curl_free(escaped_username);
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
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

	//add to chat
	curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_id = curl_easy_escape(curl, id.c_str(), id.size());
	url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/chats/" + std::string(escaped_id) +
		"?updateMask.fieldPaths=users&key=" + api;
	curl_free(escaped_id);



	json users_field = getChatField(id, "users");
	std::vector<std::string> users;
	if (!users_field.empty() && users_field.contains("arrayValue")) {
		for (const auto& item : users_field["arrayValue"]["values"]) {
			users.push_back(item["stringValue"].get<std::string>());
		}
	}

	if (std::find(users.begin(), users.end(), username) != users.end()) {
		return;
	}
	users.push_back(username);


	body = {
		{"fields", {
			{"users", {
				{"arrayValue", {
					{"values", json::array()}
				}}
			}}
		}}
	};


	for (const auto& req : users) {
		body["fields"]["users"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
	}


	headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");


	request_body = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"); // Используем PATCH вместо PUT
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	res = curl_easy_perform(curl);
	http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);


	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);


	if (http_code != 200) {
		throw std::runtime_error(
			"Server Error (" + std::to_string(http_code) + ")\n");
	}
}

std::string Client::getRequests(const std::string& username)
{
	std::vector<std::string> requests;
	json Ids = getUserField(username, "requests");
	for (auto& id : Ids["arrayValue"]["values"])
	{
		return Crypto::decryptMAC(id["stringValue"].get<std::string>(),FileService::loadFromFile(key_path));
	}
	return "";
}

void Client::addAllowedMAC(const std::string& username)
{
	
	std::string mac;
	json Ids = getUserField(username, "requests");
	for (auto& id : Ids["arrayValue"]["values"])
	{
		mac = id["stringValue"].get<std::string>();
		break;
	}
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=allowed_macs&key=" + api;
	curl_free(escaped_username);



	json requests_field = getUserField(username, "allowed_macs");
	std::vector<std::string> requests;
	if (!requests_field.empty() && requests_field.contains("arrayValue")) {
		for (const auto& item : requests_field["arrayValue"]["values"]) {
			requests.push_back(item["stringValue"].get<std::string>());
		}
	}
	for (auto& i : requests)
	{
		if (Crypto::decryptMAC(i,FileService::loadFromFile(key_path)) == Crypto::decryptMAC(mac, FileService::loadFromFile(key_path)))
			return;
	}
	requests.push_back(mac);


	json body = {
		{"fields", {
			{"allowed_macs", {
				{"arrayValue", {
					{"values", json::array()}
				}}
			}}
		}}
	};


	for (const auto& req : requests) {
		body["fields"]["allowed_macs"]["arrayValue"]["values"].push_back({ {"stringValue", req} });
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

void Client::popRequest(const std::string& username)
{
	std::vector<std::string> requests;
	json Ids = getUserField(username, "requests");
	bool pop = false;
	for (auto& id : Ids["arrayValue"]["values"])
	{
		if (!pop)
		{
			pop = true;
			continue;
		}
		requests.push_back(id["stringValue"].get<std::string>());
	}
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=requests&key=" + api;
	curl_free(escaped_username);


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

void Client::updateTime(const std::string& username)
{
	std::string curTime = std::to_string(nowTime());
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_id = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + escaped_id +
		"?updateMask.fieldPaths=lastActive&key=" + api;
	curl_free(escaped_id);

	json body = {
		{"fields", {
			{"lastActive", {
				{"stringValue", curTime}
			}}
		}}
	};
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	std::string json_data = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
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
			"Failed to update usersAmount. HTTP code: " +
			std::to_string(http_code) + "\nResponse: " + response
		);
	}
}

bool Client::isUserOnline(const std::string& username)
{
	if(getUserField(username, "lastActive").empty())
		return false;
	time_t t = std::stoll(getUserField(username, "lastActive")["stringValue"].get<std::string>());
	return nowTime() - t <= 5;
}

time_t Client::nowTime() const
{
	return time(nullptr);
}

std::vector<std::string> Client::getInvite(const std::string& username)
{
	json invites = getUserField(username, "chat_invites");

	if (!invites.contains("arrayValue") ||
		!invites["arrayValue"].contains("values") ||
		!invites["arrayValue"]["values"].is_array() ||
		invites["arrayValue"]["values"].empty()) {
		return { "", "" };
	}

	for (auto& i : invites["arrayValue"]["values"]) 
	{
		std::string chat_id = i["mapValue"]["fields"]["chat_id"]["stringValue"].get<std::string>();
		std::string sender = i["mapValue"]["fields"]["sender"]["stringValue"].get<std::string>();
		std::string key = i["mapValue"]["fields"]["key"]["stringValue"].get<std::string>();
		if (userHasChatId(sender,chat_id))
			return { chat_id,sender,key };
	}
	return { "", "" };
}

bool Client::userHasChatId(const std::string& username, const std::string& target_id)
{
	if (!isChatExists(target_id) || !isUsernameExists(username))
		return false;
	json Ids = getUserField(username, "chatsId");
	for (auto& id : Ids["arrayValue"]["values"])
	{
		std::string curId = id["stringValue"];
		if( curId== target_id)
		return true;
	}
	return false;
}

std::string Client::getChatName(const std::string& id)	
{
	return getChatField(id, "name")["stringValue"];
}

void Client::popInvite(const std::string& username) {
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_username = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + std::string(escaped_username) +
		"?updateMask.fieldPaths=chat_invites&key=" + api;
	curl_free(escaped_username);

	json invites = getUserField(username, "chat_invites");

	if (!invites.contains("arrayValue") ||
		!invites["arrayValue"].contains("values") ||
		!invites["arrayValue"]["values"].is_array() ||
		invites["arrayValue"]["values"].empty()) {
		curl_easy_cleanup(curl);
		return;
	}

	json new_array = json::array();
	auto values = invites["arrayValue"]["values"];

	for (size_t i = 1; i < values.size(); ++i) 
	{ 
		new_array.push_back(values[i]); 
	}


	json body = {
		{"fields", {
			{"chat_invites", {
				{"arrayValue", {
					{"values", new_array}
				}}
			}}
		}}
	};

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

	std::string request_body = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
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
		throw std::runtime_error("Server Error (" + std::to_string(http_code) + ")\n");
	}
}

void Client::updateKeys(const std::string& username)
{
	//Reencrypt chat key
	auto [public_key, private_key] = Crypto::generateEncryptionKeyPair(getMACs(username));
	publicKey = public_key;
	FileService::saveToFile(skey_path, private_key);

	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	char* escaped_id = curl_easy_escape(curl, username.c_str(), username.size());
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + escaped_id +
		"?updateMask.fieldPaths=public_key&key=" + api;
	curl_free(escaped_id);

	json body = {
		{"fields", {
			{"public_key", {
				{"stringValue", Crypto::base64Encode(public_key)}
			}}
		}}
	};
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	std::string json_data = body.dump();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
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
			"Failed to update usersAmount. HTTP code: " +
			std::to_string(http_code) + "\nResponse: " + response
		);
	}
}

std::vector<std::string> Client::getMACs(const std::string& username)
{
	std::vector<std::string> macs;
	json Ids = getUserField(username, "allowed_macs");
	for (auto& id : Ids["arrayValue"]["values"])
	{
		macs.push_back(id["stringValue"].get<std::string>());
	}
	return macs;
}

std::vector<std::unique_ptr<Message>> Client::getNewMessages(
	const std::string& id, time_t lastUpdateTime)
{
	json messages_field = getChatField(id, "messages");
	auto key = FileService::loadFromFile("tmp.tmp");
	if (!messages_field.contains("arrayValue") ||
		!messages_field["arrayValue"].contains("values") ||
		!messages_field["arrayValue"]["values"].is_array())
	{
		return {};
	}

	std::vector<std::unique_ptr<Message>> new_messages;
	for (auto& item : messages_field["arrayValue"]["values"])
	{
		if (!item.contains("mapValue") ||
			!item["mapValue"].contains("fields") ||
			!item["mapValue"]["fields"].contains("timestamp") ||
			!item["mapValue"]["fields"].contains("sender") ||
			!item["mapValue"]["fields"].contains("content"))
		{
			continue;
		}

		auto& ts_field = item["mapValue"]["fields"]["timestamp"];
		if (!ts_field.contains("integerValue")) continue;

		time_t timestamp;
		if (ts_field["integerValue"].is_string()) {
			timestamp = std::stoll(ts_field["integerValue"].get<std::string>());
		}
		else if (ts_field["integerValue"].is_number()) {
			timestamp = ts_field["integerValue"].get<time_t>();
		}
		else {
			continue;
		}

		if (timestamp <= lastUpdateTime) continue;

		std::string sender = Crypto::decryptSymmetric(item["mapValue"]["fields"]["sender"]["stringValue"],key);
		std::string content = Crypto::decryptSymmetric(item["mapValue"]["fields"]["content"]["stringValue"],key);

		if (item["mapValue"]["fields"].contains("format")) {
			std::string format = Crypto::decryptSymmetric(item["mapValue"]["fields"]["format"]["stringValue"],key);
			new_messages.push_back(
				std::make_unique<Image>(sender, content, format, std::to_string(timestamp))
			);
		}
		else {
			new_messages.push_back(
				std::make_unique<Text>(sender, content, std::to_string(timestamp))
			);
		}
	}
	return new_messages;
}

std::string Client::loadChat(const std::string& username, unsigned int cid)
{
	json Ids = getUserField(username, "chatsId");
	json Keys = getUserField(username, "chatsKey");
	std::string realId= Ids["arrayValue"]["values"][cid]["stringValue"];
	std::string key = Keys["arrayValue"]["values"][cid]["stringValue"];
	FileService::saveToFile("tmp.tmp", Crypto::decryptAsymmetric(Crypto::base64Decode(key),publicKey,FileService::loadFromFile(skey_path)));
	return realId;
}

void Client::addMessage(const std::string& chat_id, std::unique_ptr<Message> msg)
{
	nlohmann::json new_message;
	Text* text_msg = dynamic_cast<Text*>(msg.get());
	auto key = FileService::loadFromFile("tmp.tmp");
	if (!text_msg) {
		Image* image_msg = dynamic_cast<Image*>(msg.get());
		new_message = {
			{"mapValue", {
				{"fields", {
					{"sender", {{"stringValue", Crypto::encryptSymmetric(image_msg->getSender(),key)}}},
					{"content", {{"stringValue", Crypto::encryptSymmetric(image_msg->getData(),key)}}},
					{"timestamp", {{"integerValue", std::stol(image_msg->getTimestamp())}}},
					{"format", {{"stringValue", Crypto::encryptSymmetric(image_msg->getFormat(),key)}}}
				}}
			}}
		};
	}
	else {
		new_message = {
			{"mapValue", {
				{"fields", {
					{"sender", {{"stringValue", Crypto::encryptSymmetric(text_msg->getSender(),key)}}},
					{"content", {{"stringValue", Crypto::encryptSymmetric(text_msg->getData(),key)}}},
					{"timestamp", {{"integerValue", std::stol(text_msg->getTimestamp())}}}
				}}
			}}
		};
	}

	nlohmann::json chat_doc = getChatField(chat_id, "messages");
	nlohmann::json messages_array = nlohmann::json::array();

	if (!chat_doc.empty() &&
		chat_doc.contains("arrayValue") &&
		chat_doc["arrayValue"].contains("values"))
	{
		messages_array = chat_doc["arrayValue"]["values"];
	}

	messages_array.push_back(new_message);

	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/chats/" + chat_id +
		"?updateMask.fieldPaths=messages&key=" + api;

	nlohmann::json update_body = {
		{"fields", {
			{"messages", {
				{"arrayValue", {
					{"values", messages_array}
				}}
			}}
		}}
	};

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	std::string request_body = update_body.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
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
			"Server Error (" + std::to_string(http_code) + "): " + response + "\n");
	}
}

void Client::addInvite(const std::string& username, const std::string& id, const std::string& sender, std::vector<unsigned char> ckey)
{
	if (userHasChatId(username, id))
		throw std::exception("User already in this chat");

	// get Chats id

	json invites = getUserField(username, "chat_invites");
	json new_message;
	new_message = {
			{"mapValue", {
				{"fields", {
					{"chat_id", {{"stringValue", id}}},
					{"sender", {{"stringValue", sender}}},
					{"key", {{"stringValue", Crypto::base64Encode(Crypto::encryptAsymmetric(getPublicKey(username),ckey))}}}
				}}
			}}
	};
	if (!invites.contains("arrayValue") ||
		!invites["arrayValue"].contains("values") ||
		!invites["arrayValue"]["values"].is_array() ||
		invites["arrayValue"]["values"].empty()) {
		
	}
	else
	for (auto& i : invites["arrayValue"]["values"])
	{
		std::string chat_id = i["mapValue"]["fields"]["chat_id"]["stringValue"].get<std::string>();
		std::string sender = i["mapValue"]["fields"]["sender"]["stringValue"].get<std::string>();
		std::string key = i["mapValue"]["fields"]["key"]["stringValue"].get<std::string>();
		if(chat_id==id)
			throw std::exception("User already have invite to this chat");
	}
	
	nlohmann::json inv = nlohmann::json::array();

	if (!invites.empty() &&
		invites.contains("arrayValue") &&
		invites["arrayValue"].contains("values"))
	{
		inv = invites["arrayValue"]["values"];
	}

	inv.push_back(new_message);

	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("CURL initialization failed");
	}

	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id +
		"/databases/(default)/documents/users/" + username +
		"?updateMask.fieldPaths=chat_invites&key=" + api;

	nlohmann::json update_body = {
		{"fields", {
			{"chat_invites", {
				{"arrayValue", {
					{"values", inv}
				}}
			}}
		}}
	};

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	std::string request_body = update_body.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
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
			"Server Error (" + std::to_string(http_code) + "): " + response + "\n");
	}

}

void Client::increaseChatUsers(const std::string& id)
{
	json amount = getChatField(id, "usersAmount");
	std::string s = amount["stringValue"];
	long long kol = std::stoll(s);
	kol++;
	
	updateChatUserAmount(id, std::to_string(kol));
}
