#include "Image.h"


Image::Image(std::string sender, std::string data, std::string format, std::string timestamp)
{
	this->sender = sender;
	this->format = format;
	this->data = data;
    this->timestamp = timestamp;
}

std::string Image::to_string()
{
    return "----------------------------------------\n"+sender + ":\n[Image]\n----------------------------------------\n\n";
}