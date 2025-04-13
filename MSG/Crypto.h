#pragma once
#include <string>
class Crypto
{
	static std::pair<std::string, std::string>genKeys();
	static std::string hashPassword(); 
};

