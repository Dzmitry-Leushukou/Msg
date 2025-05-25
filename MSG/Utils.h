#pragma once
#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif

#include <vector>
#include <string>
#include <sstream>

class EXP Utils
{
	public:
		static std::string to_string(std::vector<unsigned char>);
		static bool isUInt(std::string);
	private:

};
