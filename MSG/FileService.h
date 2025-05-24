#pragma once
#include <vector>
#include <string>
#include <fstream>


class FileService
{
public:
	static std::vector<std::string> getConfigInfo(int targetAmountOfParams, std::string configPath = "D:\\Programming\\Projects\\MSG\\x64\\config");
	static void saveToFile(std::string path, std::string data);
};

