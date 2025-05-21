#pragma once
#include "Message.h"
class Text :
    public Message
{
public:
    Text(std::string, std::string, std::string);
    std::string to_string() override;
private:
    
};

