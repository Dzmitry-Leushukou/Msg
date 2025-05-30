#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <sodium.h>
#include <algorithm>
#include <array>

class Crypto
{
	public:
		static void init();
		static std::string hashPassword(const std::string& password);
        static bool verifyPassword(const std::string& password, const std::string& hash);
		static std::string encryptMAC(const std::string& mac, const std::vector<unsigned char>& key);
		static std::string decryptMAC(const std::string& base64_data, const std::vector<unsigned char>& key);
		static std::vector<unsigned char> generateKeySeed(const std::vector<std::string>& encrypted_macs_base64, const std::vector<unsigned char>& key);
		static std::pair<std::vector<unsigned char>, std::vector<unsigned char>> generateKeyPair(const std::vector<unsigned char>& seed);
		static std::vector<std::string> encryptAllMACs(const std::vector<std::string>& macs, const std::vector<unsigned char>& key);
		static std::vector<unsigned char> deriveKeyFromPassword(const std::string& password, const std::vector<unsigned char>&salt);
		static std::string base64Encode(const std::vector<unsigned char>& data);
		static std::vector<unsigned char> base64Decode(const std::string& encoded);
		static std::vector<unsigned char> generateSalt();
		static std::vector<unsigned char> generateChatKey();
		static std::pair<std::vector<unsigned char>, std::vector<unsigned char>> generateEncryptionKeyPair(const std::vector<std::string>& macs);
		static std::vector<unsigned char> generateKeySeed(const std::vector<std::string>& macs);
		static std::vector<unsigned char> encryptAsymmetric(const std::vector<unsigned char>& publicKey, const std::vector<unsigned char>& message);
		static std::vector<unsigned char> decryptAsymmetric(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& publicKey,
		const std::vector<unsigned char>& privateKey);
		static std::string encryptSymmetric(const std::string& message,const std::vector<unsigned char>& key);

		static std::string decryptSymmetric(const std::string& ciphertext,const std::vector<unsigned char>& key);
		static std::string BASE64_CHARS;

        static std::string base64Encode2(const std::string& input);
        static std::string base64Decode2(const std::string& encoded);
};

