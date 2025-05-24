#include "FileService.h"

std::vector<std::string> FileService::getConfigInfo(int targetAmountOfParams, std::string configPath)
{
	try
	{
		std::vector<std::string> data;
		std::ifstream fin(configPath);
		std::string s;

		while (targetAmountOfParams--) 
		{
			if (std::getline(fin, s))
			{
				data.push_back(s);
			}
			else
			{
				fin.close();
				throw std::invalid_argument("Invalid config file");
			}
		}
	
		fin.close();
		return data;
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file");
	}
}

void FileService::saveToFile(std::string path, std::string data)
{
	try
	{
		std::ofstream fout(path);
		fout << data;

		fout.close();
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file");
	}
}