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

bool Utils::isUInt(std::string s)
{
	for (auto& i : s)
	{
		if (i < '0' || i>'9')
			return false;
	}
	try
	{
		std::stoul(s);
		return true;
	}
	catch (...) 
	{
		return false;
	}
}