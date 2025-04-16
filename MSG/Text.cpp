#include "Text.h"

Text::Text(std::string sender, std::string data)
{
	this->sender = sender;
	this->data = data;
}

std::string Text::to_string()
{
	return sender + ": " + data;
}