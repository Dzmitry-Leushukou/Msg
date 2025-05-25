#pragma once
#include "Page.h"
class CreateChatForm :
    public Page
{
public:
    CreateChatForm(Application& app);
    virtual void start() override;
    std::string getName()const;
private:
    std::string name;
};

