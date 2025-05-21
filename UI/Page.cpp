#include "Page.h"

int Page::getInt(std::string text,int min, int max)
{
	while (true)
	{
		std::string s;
		std::cout << text;
		std::getline(std::cin, s);
		try
		{
			int res = stoi(s);
			for (int i = 0; i <s.size();i++)
			{
				if (s.at(i) == '-' && i == 0)
					continue;
				if (s.at(i) < '0' || s.at(i) >'9')
					throw std::exception();
			}
			return res;
		}
		catch (...)
		{
			std::cout << "Wrong input. Try again\n";
		}
	}
}
void Page::clearScreen()
{
	system("cls");
}
void Page::pause()
{
	system("pause");
}
std::string Page::getString(std::string text, unsigned int min)
{
	std::string s;
	while (true)
	{
		std::cout << text;
		std::getline(std::cin, s);
		if (s.size() < min)
		{
			std::cout << "Wrong input. Minimal size of input: " << min << " symbols\n";
			continue;
		}
		return s;
	}

}