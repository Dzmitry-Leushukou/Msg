#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include "Crypto.h"
#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif

class EXP FileService
{
public:
	static std::vector<std::string> getConfigInfo(int targetAmountOfParams, std::string configPath = "D:\\Programming\\Projects\\MSG\\x64\\config");
	static void saveToFile(std::string path, std::string data);
	static void saveToFile(std::string path, std::vector<unsigned char> data);
	static std::vector<unsigned char> loadFromFile(std::string path);
	static std::string saveImage(std::string data, std::string format);
	static std::string getExt(std::string filepath);
	static std::string getImageData(std::string filepath);
};


