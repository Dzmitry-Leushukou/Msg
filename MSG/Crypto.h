#pragma once
#include <string>
class Crypto
{
	public:
	static std::pair<std::string, std::string>genKeys();
	static std::string hashPassword(); 
	static std::string decrypt(std::string) { return ""; }
};

