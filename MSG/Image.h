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
    Image(std::string, std::string, std::string);
    void save(std::string);
    void open();
    std::string to_string() override;
private:
    std::string format;
    std::string filepath;
};

