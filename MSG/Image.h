#pragma once
#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif
#include "Message.h"
#include <filesystem>

class EXP Image :
    public Message
{
public:
    Image(std::string sender, std::string data, std::string format, std::string timestamp);
    std::string to_string() override;
    std::string getFormat() const
    {
        return format;
    }
private:
    std::string format;
};

