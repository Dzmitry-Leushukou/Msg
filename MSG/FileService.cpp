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
				throw std::invalid_argument("Invalid config file\n");
			}
		}
	
		fin.close();
		return data;
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file\n");
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
		throw std::invalid_argument("Invalid config file\n");
	}
}

void FileService::saveToFile(std::string path, std::vector<unsigned char> data)
{
	try
	{
		std::ofstream fout(path);
		for(auto&i:data)
			fout << (int)i<<'\n';

		fout.close();
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file\n");
	}
}
std::vector<unsigned char> FileService::loadFromFile(std::string path)
{
	try
	{
		std::vector<unsigned char> res;
		std::ifstream fin(path);
		std::string s;
		while (std::getline(fin, s))
		{
			res.push_back(std::stoi(s));
		}
		fin.close();
		return res;
	}
	catch (...)
	{
		throw std::invalid_argument("Invalid config file");
	}
}