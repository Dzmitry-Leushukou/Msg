#pragma once
#include "Page.h"
class ChatUI :
    public Page
{
public:
    ChatUI(Application& app, unsigned int num);
    virtual void start() override;
private:
    void help();
};

