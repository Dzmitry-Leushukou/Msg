#pragma once
#include "Message.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Windows.h"

class Image :
    public Message
{
public:
    Image(std::string sender, std::string data, std::string format, std::string timestamp);
    std::string to_string() override;
private:
    std::string format;
};

