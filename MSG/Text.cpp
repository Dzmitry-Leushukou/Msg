#include "Text.h"

Text::Text(std::string sender, std::string data, std::string timestamp)
{
	this->sender = sender;
	this->data = data;
	this->timestamp = timestamp;
}

std::string Text::to_string()
{
	return "----------------------------------------\n" + sender + ":\n"+data+"\n----------------------------------------\n\n";
}