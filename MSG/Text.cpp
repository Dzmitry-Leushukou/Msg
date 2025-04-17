#include "Text.h"

Text::Text(std::string sender, std::string data, std::string time)
{
	this->sender = sender;
	this->data = data;
	timestamp = time;
}

std::string Text::to_string()
{
	return sender + ": " + data;
}