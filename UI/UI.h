#pragma once

#include <iostream>
#include "User.h"
#include "LoginForm.h"

class UI
{
public:
	void start();
private:
	void authorize();
	User* user = nullptr;
};

