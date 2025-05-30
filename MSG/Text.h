#pragma once
#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif
#include "Message.h"

class EXP Text :
    public Message
{
public:
    Text(std::string sender, std::string data, std::string timestamp);
    std::string to_string() override;
private:
    
};

