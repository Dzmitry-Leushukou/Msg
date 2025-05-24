#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <sodium.h>
class Crypto
{
	public:
		static void init();
        static void generateKeyPair(const std::vector<std::string>& macs, std::vector<unsigned char>& private_key, std::vector<unsigned char>& public_key);
		static std::string hashPassword(const std::string& password);
        static bool verifyPassword(const std::string& password, const std::string& hash);
        static std::string base64Encode(const std::vector<unsigned char>& data);
};

