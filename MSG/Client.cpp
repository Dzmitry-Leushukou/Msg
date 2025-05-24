#include "Client.h"

Client::Client()
{
	try
	{
		std::ifstream fin(filepath);
		std::string s;
	
		if (std::getline(fin, s))
		{
			api = s;
		}
		else 
		{
			fin.close();
			throw std::invalid_argument("Invalid config file"); 
		}

		if (std::getline(fin, s))
		{
			fin.close();
			proj_id = s;
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
	Crypto::init();
}

std::vector<unsigned char> Client::registerUser(const std::string& username, const std::string& password, const std::vector<std::string>& macs)
{
	/*
	if (getUserDocument(username)) 
	{
		throw std::invalid_argument("Username \""+username+"\" already used");
	}
	*/
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

	if (!saveToFirestore("users", username, user_data))
		throw std::runtime_error("Can`t acces to server");
	return private_key;
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

bool Client::checkMacAddress(const json& allowed_macs, const std::string& mac)
{
	return any_of(allowed_macs.begin(), allowed_macs.end(),
		[&mac](const auto& m) { return m == mac; });
}

bool Client::saveToFirestore(const std::string& collection, const std::string& doc_id, const json& data)
{
	
	std::string url = "https://firestore.googleapis.com/v1/projects/" + proj_id + "/databases/(default)/documents/" + collection + "/" + doc_id + "?key=" + api;

	json firestore_doc;
	firestore_doc["fields"] = json::object();

	for (auto& [key, value] : data.items())
	{

		if (key == "chat_invites")
		{
			json invites_array;
			for (auto& invite : value)
			{
				json invite_map;
				invite_map["mapValue"] = {
					{"fields", {
						{"chat_id", {{"stringValue", invite["chat_id"]}},
						{"sender", {{"stringValue", invite["sender"]}}
					}}
				}} };
				invites_array.push_back(invite_map);
			}
			firestore_doc["fields"][key] = { {"arrayValue", {{"values", invites_array}}} };
		}
		else if (value.is_array())
		{
			json array_values;
			for (auto& item : value) {
				array_values.push_back({ {"stringValue", item} });
			}
			firestore_doc["fields"][key] = { {"arrayValue", {{"values", array_values}}} };
		}
		else if (value.is_string())
		{
			firestore_doc["fields"][key] = { {"stringValue", value} };
		}
	}

	curl = curl_easy_init();
	std::string json_data = firestore_doc.dump();
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CURLcode res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
		return false;
	}

	if (http_code != 200) {
		std::cerr << "Firestore error. HTTP code: " << http_code << std::endl;
		return false;
	}

	return true;
}