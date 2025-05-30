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
std::string FileService::saveImage(std::string data, std::string format)
{
	unsigned long long numb = 0;
	while (true)
	{
		std::string filepath = "cache" + std::to_string(numb) + format;
		std::ifstream check(filepath);
		if (check.good())
		{
			numb++;
			check.close();
			continue;
		}
		check.close();
		std::ofstream file(filepath, std::ios::binary);

		if (!file) 
		{
			throw std::exception("Can`t write to this file");
		}
		std::vector<unsigned char>raw = Crypto::base64Decode(data);
		file.write(reinterpret_cast<const char*>(raw.data()), raw.size());

		if (!file)
		{
			throw std::exception("Can`t write to this file");
		}

		file.close();

		std::filesystem::path p(filepath);
		return std::filesystem::absolute(p).string();
	}
}

std::string FileService::getExt(std::string filepath)
{
	std::filesystem::path p(filepath);

	if (!std::filesystem::exists(p)) {
		throw std::runtime_error("File doesn`t exist: " + filepath);
	}
	std::string extension = p.extension().string();

	if (extension.empty()) {
		throw std::runtime_error("Extension doesn`t found: " + filepath);
	}
	return extension;
}

std::string FileService::getImageData(std::string filepath)
{
	std::ifstream file(filepath, std::ios::binary);

	if (!file) 
	{
		throw std::exception("Can`t read the file data");
	}

	std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return Crypto::base64Encode(buffer);
}