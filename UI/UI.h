#pragma once

#include <iostream>
#include "Application.h"
#include "LoginForm.h"
#include "MainMenu.h"
class UI
{
public:
	~UI();
	void start();
private:
	void authorize();
	
	Application* app = new Application();
};

