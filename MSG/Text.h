#pragma once
#include "Message.h"
class Text :
    public Message
{
public:
    Text(std::string sender, std::string data, std::string timestamp);
    std::string to_string() override;
private:
    
};

