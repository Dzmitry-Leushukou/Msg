#pragma once
#include <vector>
#include <string>
#include <sstream>
class Utils
{
	public:
		static std::vector<unsigned char>MACsToUCharVector(std::vector<std::string> MACs);

	private:
		static std::vector<unsigned char> parseMAC(const std::string& mac_str);

};

