#include "Utils.h"

std::string Utils::to_string(std::vector<unsigned char>v)
{
	std::string ans;
	for (auto& i : v)
		ans += std::to_string((unsigned int)i) + " ";
	if(!ans.empty())
		ans.pop_back();
	return ans;
}