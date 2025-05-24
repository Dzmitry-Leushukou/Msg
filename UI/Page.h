#pragma once
#include <string>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
class Page
{
public:
	virtual ~Page() {};
	virtual void start() = 0;
protected:
	//Input methods
	int getInt(std::string,int min,int max);
	std::string getString(std::string, unsigned int min = 1);
	//Visual methods
	void clearScreen();
	void pause();
};

